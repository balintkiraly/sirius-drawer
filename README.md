# Sirius drawer

## Building with CMake

Build the `Makefile` with the following command!
```
cmake .
```

After that you can run the make command to build the app.
```
make
```

## Specification

A Szíriusz csillagképből érkező földönkívülieknek megtetszett a Word, Powerpoint, stb. 2D-s rajzoló programja, de azt közvetlenül nem használhatják, ugyanis ők más geometriával dolgoznak. Önt bízták meg a program adaptálásával arra az egyszerű esetre, amikor három pontra egy háromszöget kell illeszteni. Szerencsére adtak egy szótárat, ami a szíriuszi geometriai fogalmakat megfelelteti az általunk használt euklideszi geometriai fogalmaknak:

A Szíriusz sík az euklideszi sík egység sugarú köre, amit alapkörnek neveznek.

Miközben az euklideszi síkot a komplex számokkal lehet számszerűsíteni, a Szíriusz síkot az egységnél kisebb abszolút értékű komplex számokkal. Amíg az euklideszi sík metrikája |dz|=sqrt(dx^2+dy^2) a Szíriusz síké |dz|/(1-|z|^2).

A Szíriusz egyenes egy olyan euklideszi körív, amely az alapkörre merőlegesen érkezik.

A feladat három egér klikk után a három pont által definiált háromszöget a háttértől eltérő színnel kitölteni, az éleket ezektől eltérő színnel felrajzolni, és a szabványos kimenetre a három szögeit és oldalainak hosszát kiírni.

