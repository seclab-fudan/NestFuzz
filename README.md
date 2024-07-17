# NestFuzz

NestFuzz is a structure-aware grey box fuzzer that developed based on AFL. It mainly includes two phases. 
In the first phase of input processing logic modeling, NestFuzz first leverages taint analysis to identify input-accessing instructions. Then, NestFuzz recognizes the inter-field dependencies and hierarchy dependencies by understanding the control- and data-flow relationships between these input-accessing instructions. Last, NestFuzz proposes a novel data structure, namely Input Processing Tree, that can represent the whole structure of the input format.
In the second phase of fuzzing, NestFuzz designs a cascading dependency-aware mutation strategy. Based on the recognized dependencies, whenever NestFuzz mutates (field or structure-level) the input, it cascadingly mutates other affected fields or substructures to maintain the structure validity. Therefore, NestFuzz can continuously and effectively generate new high-quality test cases.

For more details, welcome to follow our [paper](https://dl.acm.org/doi/abs/10.1145/3576915.3623103).
If you use NestFuzz in your science work, please use the following BibTeX entry:
```
@inproceedings{deng2023nestfuzz,
  title={NestFuzz: Enhancing Fuzzing with Comprehensive Understanding of Input Processing Logic},
  author={Deng, Peng and Yang, Zhemin and Zhang, Lei and Yang, Guangliang and Hong, Wenzheng and Zhang, Yuan and Yang, Min},
  booktitle={Proceedings of the 2023 ACM SIGSAC Conference on Computer and Communications Security},
  pages={1272--1286},
  year={2023}
}
```
