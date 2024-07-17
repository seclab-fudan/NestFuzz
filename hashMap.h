#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stdint.h>
#include "chunk.h"

typedef struct entry {
	uint8_t *key;
	Chunk *value;
	struct entry * next;
}*Entry;

#define newEntry() (Entry)malloc(sizeof(struct entry))
#define newEntryList(length) (Entry)malloc(length * sizeof(struct entry))

enum _Boolean { True = 1, False = 0 };
typedef enum _Boolean Boolean;

// ��ϣ�ṹ
typedef struct hashMap *HashMap;

#define newHashMap() (HashMap)malloc(sizeof(struct hashMap))

// ��ϣ��������
typedef uint32_t(*HashCode)(HashMap, uint8_t *key);

// �еȺ�������
typedef Boolean(*Equal)(uint8_t *key1, uint8_t *key2);

// ��Ӽ���������
typedef void(*Put)(HashMap hashMap, uint8_t *key, Chunk *value);

// ��ȡ����Ӧֵ�ĺ�������
typedef Chunk *(*Get)(HashMap hashMap, uint8_t *key);

// ɾ�����ĺ�������
typedef uint8_t *(*Remove)(HashMap hashMap, uint8_t *key);

// ���Map�ĺ�������
typedef void(*Clear)(HashMap hashMap);

// �жϼ�ֵ�Ƿ���ڵĺ�������
typedef Boolean(*Exists)(HashMap hashMap, uint8_t *key);

typedef struct hashMap {
	uint32_t size;			// ��ǰ��С
	uint32_t listSize;		// ��Ч�ռ��С
	HashCode hashCode;	// ��ϣ����
	Equal equal;		// �еȺ���
	Entry list;			// �洢����
	Put put;			// ��Ӽ��ĺ���
	Get get;			// ��ȡ����Ӧֵ�ĺ���
	Remove remove;		// ɾ����
	Clear clear;		// ���Map
	Exists exists;		// �жϼ��Ƿ����
	Boolean autoAssign;	// �趨�Ƿ���ݵ�ǰ��������̬�����ڴ��С��Ĭ�Ͽ���
}*HashMap;

// �������ṹ
typedef struct hashMapIterator {
	Entry entry;	// ��������ǰָ��
	uint32_t count;		// ��������
	uint32_t hashCode;	// ��ֵ�ԵĹ�ϣֵ
	HashMap hashMap;
}*HashMapIterator;

#define newHashMapIterator() (HashMapIterator)malloc(sizeof(struct hashMapIterator))

// Ĭ�Ϲ�ϣ����
uint32_t defaultHashCode(HashMap hashMap, uint8_t *key);

// Ĭ���жϼ�ֵ�Ƿ����
Boolean defaultEqual(uint8_t *key1, uint8_t *key2);

// Ĭ����Ӽ�ֵ��
void defaultPut(HashMap hashMap, uint8_t *key, Chunk *value);

// Ĭ�ϻ�ȡ����Ӧֵ
Chunk *defaultGet(HashMap hashMap, uint8_t *key);

// Ĭ��ɾ����
uint8_t *defaultRemove(HashMap hashMap, uint8_t *key);

// Ĭ���жϼ��Ƿ����
Boolean defaultExists(HashMap hashMap, uint8_t *key);

// Ĭ�����Map
void defaultClear(HashMap hashMap);

// ���¹���
void resetHashMap(HashMap hashMap, uint32_t listSize);

// ����һ����ϣ�ṹ
HashMap createHashMap(HashCode hashCode, Equal equal);

// ������ϣ�ṹ������
HashMapIterator createHashMapIterator(HashMap hashMap);

// �������Ƿ�����һ��
Boolean hasNextHashMapIterator(HashMapIterator iterator);

// ��������һ��
HashMapIterator nextHashMapIterator(HashMapIterator iterator);

// �ͷŵ������ڴ�
void freeHashMapIterator(HashMapIterator * iterator);

#endif // !__HASHMAP_H__