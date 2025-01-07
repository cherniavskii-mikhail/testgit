# A Computational Study of the Tool Replacement Problem

This archive is distributed in association with the [INFORMS Journal on
Computing](https://pubsonline.informs.org/journal/ijoc) under the [MIT License](LICENSE).

The software and data in this repository are a snapshot of the software and data
that were used in the research reported on in the paper 
[This is a Template](https://doi.org/10.1287/ijoc.2023.0474) by Yuzhuo Qiu, Mikhail Cherniavskii, Boris Goldengorin, Panos M. Pardalos.  

## Cite

To cite the contents of this repository, please cite both the paper and this repo, using their respective DOIs.

https://doi.org/10.1287/ijoc.2023.0474

https://doi.org/10.1287/ijoc.2023.0474.cd

Below is the BibTex for citing this snapshot of the repository.

```
@misc{CacheTest,
  author =        {Yuzhuo Qiu, Mikhail Cherniavskii, Boris Goldengorin, Panos M. Pardalos},
  publisher =     {INFORMS Journal on Computing},
  title =         {A Computational Study of the Tool Replacement Problem},
  year =          {2023},
  doi =           {https://doi.org/10.1287/ijoc.2023.0474.cd},
  note =          {Available for download at https://github.com/INFORMSJoC/2023.0474},
} 
```

## Description

The goal of this software is compare to IGA and KTNS algorithms for the Tool Replacement Problem.

## Building

In Linux, to build the version that multiplies all elements of a vector by a
constant (used to obtain the results in [Figure 1](results/mult-test.png) in the
paper), stepping K elements at a time, execute the following commands.

```
make mult
```

Alternatively, to build the version that sums the elements of a vector (used
to obtain the results [Figure 2](results/sum-test.png) in the paper), stepping K
elements at a time, do the following.

```
make clean
make sum
```

Be sure to make clean before building a different version of the code.

## Results

Figure 1 in the paper shows the results of the multiplication test with different
values of K using `gcc` 7.5 on an Ubuntu Linux box.

![Figure 1](results/mult-test.png)

Figure 2 in the paper shows the results of the sum test with different
values of K using `gcc` 7.5 on an Ubuntu Linux box.

![Figure 1](results/sum-test.png)

## Replicating

To replicate the results in [Figure 1](results/mult-test), do either

```
make mult-test
```
or
```
python test.py mult
```
To replicate the results in [Figure 2](results/sum-test), do either

```
make sum-test
```
or
```
python test.py sum
```

