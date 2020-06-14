# laboratory 1

##### Timpii de executie a aplicatiei de baza secventiala.
  - Sleepy: 12.661 s  
  - Busy: 10.641 s

__Numarul de nuclee pe calculator - **6**__

##### Timpii de execuţie a aplicaţiei scrise

Cu Open**MP**
| Număr fire | 1 | 2 | 4 | 8 | 16 | 32 |
| ------ | ------ | ------ | ------ | ------ | ------ | ------ |
| Sleepy(s) | 12.69 | 6.33 | 3.52 | 3.05 | 2.31 | 5.81 |
| Busy(s) | 11.77 | 5.97 | 3.27 | 2.89 | 2.37 | 5.52 |


Cu Open**MPI**
| Număr fire | 1 | 2 | 4 | 8 | 16 | 32 |
| ------ | ------ | ------ | ------ | ------ | ------ | ------ |
| Sleepy(s) | 23.81 | |  | | | |
| Busy(s) | 11.67 | | | | | |


#### Consideratii
Dupa timpii de executie cu ajutorul la OpenMP observat ca este de ajuns 16 fire de executie pentru a atinge timpul minim. Plus logica de separare a numerelor per fir de executie putea fi linear descrescator din cauza ca ultimile numere necesita mai mult timp pentru a fi executate, dar aceasta din urma nu a fost luata in consideratie.
