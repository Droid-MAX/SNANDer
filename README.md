SNANDer
===

SNANDer - Serial Nor/nAND/Eeprom programmeR (based on CH341A)

![WinChipHead CH341A based programmer](https://raw.githubusercontent.com/Droid-MAX/SNANDer/master/photos/programmer.jpg)

**Requirements**

* gcc/mingw-w64, wget, make, and libusb-1.0-dev (via apt Command Line Tools)

**Compiling for Linux**

After installing the gcc and necessary tools, building `snander` is as simple as running the command:

```
make -C src/
```

After successful compilation, the target executable file will be generated in the current folder.

**Compiling for Windows**

After installing the mingw-w64 and necessary tools, building `snander` is as simple as running the included script:

```
./build.sh
```

After successful compilation, the target executable file will be generated in the `build` folder.

**Usage**

Using `snander` is straightforward:

```
SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.6.2 by McMCC <mcmcc@mail.ru>

  Usage:
 -h             display this message
 -d             disable internal ECC(use read and write page size + OOB size)
 -I             ECC ignore errors(for read test only)
 -L             print list support chips
 -i             read the chip ID info
 -E             select I2C EEPROM {24c01|24c02|24c04|24c08|24c16|24c32|24c64|24c128|24c256|24c512|24c1024}
                select Microwire EEPROM {93c06|93c16|93c46|93c56|93c66|93c76|93c86|93c96} (need SPI-to-MW adapter)
 -8             set organization 8-bit for Microwire EEPROM(default 16-bit) and set jumper on SPI-to-MW adapter
 -f <addr len>  set manual address size in bits for Microwire EEPROM(default auto)
 -e             erase chip(full or use with -a [-l])
 -l <bytes>     manually set length
 -a <address>   manually set address
 -w <filename>  write chip with data from filename
 -r <filename>  read chip and save data to filename
 -v             verify after write on chip
```

For example:

1. Get Flash info.

```
$ ./SNANDer -i

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.6.2 by McMCC <mcmcc@mail.ru>

Found programmer device: Winchiphead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xef, dev_id = 0xaa
Get Status Register 1: 0x81
Get Status Register 2: 0x18
Using Flash ECC.
Detected SPI NAND Flash: WINBOND W25N01G, Flash Size: 128 MB
$
```

2. Full erase flash with disable internal ECC check. Without OOB, page size 2112 bytes.

```
$ ./SNANDer -d -e

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.6.2 by McMCC <mcmcc@mail.ru>

Found programmer device: Winchiphead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xef, dev_id = 0xaa
Get Status Register 1: 0x81
Get Status Register 2: 0x18
Disable Flash ECC.
Detected SPI NAND Flash: WINBOND W25N01G, Flash Size: 128 MB
ERASE:
Set full erase chip!
Erase addr = 0x0000000000000000, len = 0x0000000008400000
Status: OK
$
```

3. Write and verify flash with disable internal ECC check. Without OOB, page size 2112 bytes.

```
$ ./SNANDer -d -v -w ecc_1Gb_2K_64_flashimage_rfb1_ac2600.bin 

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.6.2 by McMCC <mcmcc@mail.ru>

Found programmer device: Winchiphead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xef, dev_id = 0xaa
Get Status Register 1: 0x81
Get Status Register 2: 0x08
Disable Flash ECC.
Detected SPI NAND Flash: WINBOND W25N01G, Flash Size: 128 MB
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000001080000
snand_erase_write: offs:0, count:1080000
..........................................................................................................
..........................................................................................................
..........................................................................................................
..........................................................................................................
........................................................................................................Done!
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000001080000
Status: OK
$
```

4. Read Microwire EEPROM Atmel 93C46 and save file.

```
$ ./SNANDer -E 93c46 -r test.bin

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.6.2 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xff, dev_id = 0xff
SPI NAND Flash Not Detected!
spi device id: ff ff ff ff ff (ffffffff)
SPI NOR Flash Not Detected!
I2C EEPROM Not Detected!
Microwire EEPROM chip: 93c46, Size: 64 bytes, Org: 16 bits, fix addr len: Auto
READ:
Read addr = 0x0000000000000000, len = 0x0000000000000080
............................................................
Read [128] bytes from [93c46] EEPROM address 0x00000000
Status: OK
```

5. Write and verify Microwire EEPROM Atmel 93C46 from file.

```
$ ./SNANDer -E 93c46 -w test.bin -v

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.6.2 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xff, dev_id = 0xff
SPI NAND Flash Not Detected!
spi device id: ff ff ff ff ff (ffffffff)
SPI NOR Flash Not Detected!
I2C EEPROM Not Detected!
Microwire EEPROM chip: 93c46, Size: 64 bytes, Org: 16 bits, fix addr len: Auto
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000000000080
............................................................
Wrote [128] bytes to [93c46] EEPROM address 0x00000000
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000000000080
............................................................
Read [128] bytes from [93c46] EEPROM address 0x00000000
Status: OK
```

**Supported Chips**

```
SPI NAND Flash Support List:
001. GIGADEVICE GD5F1GQ4UA
002. GIGADEVICE GD5F1GQ4UB
003. GIGADEVICE GD5F1GQ4UC
004. GIGADEVICE GD5F1GQ4UE
005. GIGADEVICE GD5F2GQ4UB
006. GIGADEVICE GD5F2GQ4UE
007. GIGADEVICE GD5F2GQ4UC
008. GIGADEVICE GD5F4GQ4UB
009. GIGADEVICE GD5F4GQ4UC
010. ESMT F50L512
011. ESMT F50L1G
012. ESMT F50L1G41LB
013. ESMT F50L2G41LB
014. WINBOND W25N01G
015. WINBOND W25M02G
016. MXIC MX35LF1G
017. MXIC MX35LF2G
018. ZENTEL A5U12A21ASC
019. ZENTEL A5U1GA21BWS
020. ETRON EM73C044SNB
021. ETRON EM73C044SND
022. ETRON EM73C044SNF
023. ETRON EM73C044VCA
024. ETRON EM73C044VCD
025. ETRON EM73D044VCA
026. ETRON EM73D044VCB
027. ETRON EM73D044VCD
028. ETRON EM73D044VCG
029. ETRON EM73D044VCH
030. ETRON EM73D044SNA
031. ETRON EM73D044SNC
032. ETRON EM73D044SND
033. ETRON EM73D044SNF
034. ETRON EM73E044SNA
035. TOSHIBA TC58CVG0S3H
036. TOSHIBA TC58CVG1S3H
037. TOSHIBA TC58CVG2S0H
038. MICRON MT29F1G01
039. MICRON MT29F2G01
040. MICRON MT29F4G01
041. HEYANG HYF1GQ4UAACAE
042. HEYANG HYF2GQ4UAACAE
043. HEYANG HYF2GQ4UHCCAE
044. HEYANG HYF1GQ4UDACAE
045. HEYANG HYF2GQ4UDACAE
046. PN PN26G01A-X
047. PN PN26G02A-X
048. ATO ATO25D1GA
049. ATO ATO25D2GA
050. ATO ATO25D2GB
051. FM FM25S01
052. FM FM25S01A
053. FM FM25G01B
054. FM FM25G02B
055. FM FM25G02C
056. FM FM25G02
057. XTX XT26G02B
058. XTX XT26G01A
059. XTX XT26G02A
060. MIRA PSU1GS20BN
061. BIWIN BWJX08U
062. BIWIN BWET08U
063. FORESEE FS35ND01GD1F1
064. FORESEE FS35ND01GS1F1
065. FORESEE FS35ND02GS2F1
066. FORESEE FS35ND02GD1F1
067. DS DS35Q2GA
068. DS DS35Q1GA
069. FISON CS11G0T0A0AA
070. FISON CS11G1T0A0AA
071. FISON CS11G0G0A0AA
072. TYM TYM25D2GA01
073. TYM TYM25D2GA02
074. TYM TYM25D1GA03

SPI NOR Flash Support List:
001. AT25DF321
002. AT26DF161
003. F25L016
004. F25L16QA
005. F25L032
006. F25L32QA
007. F25L064
008. F25L64QA
009. GD25Q16
010. GD25Q32
011. GD25Q64CSIG
012. GD25Q128CSIG
013. GD25Q256CSIG
014. MX25L1605D
015. MX25L3205D
016. MX25L6405D
017. MX25L12805D
018. MX25L25635E
019. MX25L51245G
020. FL016AIF
021. FL064AIF
022. S25FL032P
023. S25FL064P
024. S25FL128P
025. S25FL129P
026. S25FL256S
027. S25FL116K
028. S25FL132K
029. S25FL164K
030. EN25F16
031. EN25Q16
032. EN25QH16
033. EN25Q32B
034. EN25F32
035. EN25F64
036. EN25Q64
037. EN25QA64A
038. EN25QH64A
039. EN25Q128
040. EN25QA128A
041. EN25QH128A
042. W25X05
043. W25X10
044. W25X20
045. W25X40
046. W25X80
047. W25X16
048. W25X32VS
049. W25X64
050. W25Q20CL
051. W25Q20BW
052. W25Q20EW
053. W25Q80
054. W25Q80BL
055. W25Q32BV
056. W25Q32DW
057. W25Q64BV
058. W25Q64DW
059. W25Q128BV
060. W25Q128FW
061. W25Q256FV
062. W25Q512JV
063. M25P016
064. N25Q032A
065. N25Q064A
066. M25P128
067. N25Q128A
068. XM25QH32B
069. XM25QH32A
070. XM25QH64A
071. XM25QH128A
072. N25Q256A
073. MT25QL512AB
074. ZB25VQ16
075. ZB25VQ32
076. ZB25VQ64
077. ZB25VQ128
078. BY25Q16BS
079. BY25Q32BS
080. BY25Q64AS
081. BY25Q128AS
082. XT25F32B
083. XT25F32B
084. XT25F64B
085. XT25F128B
086. PM25LQ016
087. PM25LQ032
088. PM25LQ064
089. PM25LQ128
090. IC25LP016
091. IC25LP032
092. IC25LP064
093. IC25LP128
094. FS25Q016
095. FS25Q032
096. FS25Q064
097. FS25Q128
098. FM25W16
099. FM25W32
100. FM25W64
101. FM25W128
102. FM25Q16A
103. FM25Q32A
104. FM25Q64A
105. FM25Q128A
106. PN25F16
107. PN25F32
108. PN25F64
109. PN25F128
110. P25Q16H
111. P25Q32H
112. P25Q64H
113. P25Q128H

I2C EEPROM Support List:
001. 24c01
002. 24c02
003. 24c04
004. 24c08
005. 24c16
006. 24c32
007. 24c64
008. 24c128
009. 24c256
010. 24c512
011. 24c1024

Microwire EEPROM Support List:
001. 93c06
002. 93c16
003. 93c46
004. 93c56
005. 93c66
006. 93c76
007. 93c86
008. 93c96
```

**Author**

Originally written by [McMCC](https://github.com/McMCCRU/SNANDer) and released under the terms of the GNU GPL, version 2.0, or later. Modifications by [Droid-MAX](https://github.com/Droid-MAX/) to make it more easier build for Windows.

**License**

This is free software: you can redistribute it and/or modify it under the terms of
the latest GNU General Public License as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.
