# xx

xx is a binary stream read / write utility for shell scripting and other joys of life.

## Example usage

    $ xx --char "xx is nice" >example

    $ cat example
    xx is nice

    $ xx --int32 12345 --uint8 1 2 3 4 5 >example

    $ xxd example
    00000000: 3930 0000 0102 0304 05                   90.......

    $ xx -r --int32 --uint8 5 <example
    12345
    1 2 3 4 5

    $ xx --uint16 65535 |xx -r --int16
    -1

Etc.

*Note: [xxd(1)](http://linux.die.net/man/1/xxd) is not part of this project.*

## License

![](https://www.gnu.org/graphics/agplv3-155x51.png)

xx is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

## Exclusion of warranty

xx is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

A copy of AGPLv3 can be found in [COPYING.](COPYING)
