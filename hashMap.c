#include "hashMap.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

uint32_t defaultHashCode(HashMap hashMap, uint8_t *key) {
  uint8_t *k = key;
  uint64_t h = 0;
  while (*k) {
    h = (h << 4) + *k++;
    uint64_t g = h & 0xF0000000L;
    if (g) {
      h ^= g >> 24;
    }
    h &= ~g;
  }
  return h % hashMap->listSize;
}

Boolean defaultEqual(uint8_t *key1, uint8_t *key2) {
	return strcmp(key1, key2) ? False : True;
}

void resetHashMap(HashMap hashMap, uint32_t listSize) {

	if (listSize < 8) return;

	// ��ֵ����ʱ�洢�ռ�
	Entry tempList = newEntryList(hashMap->size);

	HashMapIterator iterator = createHashMapIterator(hashMap);
	uint32_t length = hashMap->size;
	for (uint32_t index = 0; hasNextHashMapIterator(iterator); index++) {
		// ����ȡ�����м�ֵ��
		iterator = nextHashMapIterator(iterator);
		tempList[index].key = iterator->entry->key;
		tempList[index].value = iterator->entry->value;
		tempList[index].next = NULL;
	}
	freeHashMapIterator(&iterator);

	// ���ԭ�м�ֵ������
	hashMap->size = 0;
	for (uint32_t i = 0; i < hashMap->listSize; i++) {
		Entry current = &hashMap->list[i];
		current->key = NULL;
		current->value = NULL;
		if (current->next != NULL) {
			while (current->next != NULL) {
				Entry temp = current->next->next;
				free(current->next);
				current->next = temp;
			}
		}
	}

	// �����ڴ��С
	hashMap->listSize = listSize;
	Entry relist = (Entry)realloc(hashMap->list, hashMap->listSize * sizeof(struct entry));
	if (relist != NULL) {
		hashMap->list = relist;
		relist = NULL;
	}

	// ��ʼ������
	for (uint32_t i = 0; i < hashMap->listSize; i++) {
		hashMap->list[i].key = NULL;
		hashMap->list[i].value = NULL;
		hashMap->list[i].next = NULL;
	}

	// �����м�ֵ������д���ڴ�
	for (uint32_t i = 0; i < length; i++) {
		hashMap->put(hashMap, tempList[i].key, tempList[i].value);
	}
	free(tempList);
}

void defaultPut(HashMap hashMap, uint8_t *key, Chunk *value) {
	uint32_t index = hashMap->hashCode(hashMap, key);
	if (hashMap->list[index].key == NULL) {
		hashMap->size++;
		// �õ�ַΪ��ʱֱ�Ӵ洢
		hashMap->list[index].key = key;
		hashMap->list[index].value = value;
	}
	else {

		Entry current = &hashMap->list[index];
		while (current != NULL) {
			if (hashMap->equal(key, current->key)) {
				// ���ڼ�ֵ�Ѿ����ڵ�ֱ�Ӹ���
				current->value = value;
				return;
			}
			current = current->next;
		};

		// ������ͻ�򴴽��ڵ�ҵ���Ӧλ�õ�next��
		Entry entry = newEntry();
		entry->key = key;
		entry->value = value;
		entry->next = hashMap->list[index].next;
		hashMap->list[index].next = entry;
		hashMap->size++;
	}

	if (hashMap->autoAssign && hashMap->size >= hashMap->listSize) {

		// �ڴ�������ԭ��������
		// *ע: ����ʱ���ǵ��ǵ�ǰ�洢Ԫ��������洢�ռ�Ĵ�С��ϵ�������Ǵ洢�ռ��Ƿ��Ѿ�������
		// ����: �洢�ռ�Ϊ10��������10����ֵ�ԣ�����ȫ����ͻ�ˣ����Դ洢�ռ����9���������ȫ������һ�����棬
		// ����������ʱ��ͱ�����ѯû��ʲô�����ˣ����Լ�������⣬���Ҵ����11����ֵ�Ե�ʱ��һ���ᷢ����ͻ��
		// �����ɹ�ϣ�������������(ȡģ)�����ģ���ͻ�ͻᵼ�¼���������������ʱ������洢�ռ䣬��ԭ�м�ֵ�Խ���
		// �ٴ�ɢ�У���ѳ�ͻ�������ٴη�ɢ�����ӿ�������λ�ٶȡ�
		resetHashMap(hashMap, hashMap->listSize * 2);
	}
}

Chunk *defaultGet(HashMap hashMap, uint8_t *key) {
	if (hashMap->exists(hashMap, key)) {
		uint32_t index = hashMap->hashCode(hashMap, key);
		Entry entry = &hashMap->list[index];
		while (entry != NULL) {
			if (hashMap->equal(entry->key, key)) {
				return entry->value;
			}
			entry = entry->next;
		}
	}
	return NULL;
}

uint8_t *defaultRemove(HashMap hashMap, uint8_t *key) {
	uint32_t index = hashMap->hashCode(hashMap, key);
	Entry entry = &hashMap->list[index];
	if (entry->key == NULL) {
		return NULL;
	}
    uint8_t *entryKey = entry->key;
	Boolean result = False;
	if (hashMap->equal(entry->key, key)) {
		hashMap->size--;
		if (entry->next != NULL) {
			Entry temp = entry->next;
			entry->key = temp->key;
			entry->value = temp->value;
			entry->next = temp->next;
			free(temp);
		}
        else {
            entry->key = NULL;
            entry->value = NULL;
        }
		result = True;
	}
	else {
		Entry p = entry;
		entry = entry->next;
		while (entry != NULL) {
			if (hashMap->equal(entry->key, key)) {
				hashMap->size--;
				p->next = entry->next;
				free(entry);
				result = True;
				break;
			}
			p = entry;
			entry = entry->next;
		};
	}

	// ����ռ�ռ�ò���һ�룬���ͷŶ����ڴ�
	if (result && hashMap->autoAssign &&  hashMap->size < hashMap->listSize / 2) {
		resetHashMap(hashMap, hashMap->listSize / 2);
	}
	return entryKey;
}

Boolean defaultExists(HashMap hashMap, uint8_t *key) {
	uint32_t index = hashMap->hashCode(hashMap, key);
	Entry entry = &hashMap->list[index];
	if (entry->key == NULL) {
		return False;
	}
	else {
		while (entry != NULL) {
			if (hashMap->equal(entry->key, key)) {
				return True;
			}
			entry = entry->next;
		}
		return False;
	}
}

void defaultClear(HashMap hashMap) {
	for (uint32_t i = 0; i < hashMap->listSize; i++) {
		// �ͷų�ͻֵ�ڴ�
		Entry entry = hashMap->list[i].next;
		while (entry != NULL) {
			Entry next = entry->next;
			free(entry);
			entry = next;
		}
		hashMap->list[i].next = NULL;
	}
	// �ͷŴ洢�ռ�
	free(hashMap->list);
	hashMap->list = NULL;
	hashMap->size = -1;
	hashMap->listSize = 0;
}

HashMap createHashMap(HashCode hashCode, Equal equal) {
	HashMap hashMap = newHashMap();
	if (hashMap == NULL) {
		return NULL;
	}
	hashMap->size = 0;
	hashMap->listSize = 8;
	hashMap->hashCode = hashCode == NULL ? defaultHashCode : hashCode;
	hashMap->equal = equal == NULL ? defaultEqual : equal;
	hashMap->exists = defaultExists;
	hashMap->get = defaultGet;
	hashMap->put = defaultPut;
	hashMap->remove = defaultRemove;
	hashMap->clear = defaultClear;
	hashMap->autoAssign = True;
	
	// ��ʼ����8���ڴ�ռ䣬���ʱ���Զ�����
	hashMap->list = newEntryList(hashMap->listSize);
	if (hashMap->list == NULL) {
		return NULL;
	}
	Entry p = hashMap->list;
	for (uint32_t i = 0; i < hashMap->listSize; i++) {
		p[i].key = NULL;
		p[i].value = NULL;
		p[i].next = NULL;
	}
	return hashMap;
}

HashMapIterator createHashMapIterator(HashMap hashMap) {
	HashMapIterator iterator = newHashMapIterator();
	if (iterator == NULL) {
		return NULL;
	}
	iterator->hashMap = hashMap;
	iterator->count = 0;
	iterator->hashCode = -1;
	iterator->entry = NULL;
	return iterator;
}

Boolean hasNextHashMapIterator(HashMapIterator iterator) {
	return iterator->count < iterator->hashMap->size ? True : False;
}

HashMapIterator nextHashMapIterator(HashMapIterator iterator) {
	if (hasNextHashMapIterator(iterator)) {
		if (iterator->entry != NULL && iterator->entry->next != NULL) {
			iterator->count++;
			iterator->entry = iterator->entry->next;
			return iterator;
		}
		while (++iterator->hashCode < iterator->hashMap->listSize) {
			Entry entry = &iterator->hashMap->list[iterator->hashCode];
			if (entry->key != NULL) {
				iterator->count++;
				iterator->entry = entry;
				break;
			}
		}
	}
	return iterator;
}

void freeHashMapIterator(HashMapIterator * iterator) {
	free(*iterator);
	*iterator = NULL;
}