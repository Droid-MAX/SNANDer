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
SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.4 by McMCC <mcmcc@mail.ru>

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

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.4 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xc2, dev_id = 0x22
Get Status Register 1: 0x38
Get Status Register 2: 0x10
Using Flash ECC.
Detected SPI NAND Flash: MXIC MX35LF2G, Flash Size: 256 MB
$
```

2. Full erase flash with disable internal ECC check. Without OOB, page size 2112 bytes.

```
$ ./SNANDer -d -e

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.4 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xc2, dev_id = 0x22
Get Status Register 1: 0x00
Get Status Register 2: 0x11
Disable Flash ECC.
Detected SPI NAND Flash: MXIC MX35LF2G, Flash Size: 256 MB
ERASE:
Set full erase chip!
Erase addr = 0x0000000000000000, len = 0x0000000010800000
Erase 100% [276824064] of [276824064] bytes
Elapsed time: 3 seconds
Status: OK
$
```

3. Write and verify flash with disable internal ECC check. Without OOB, page size 2112 bytes.

```
$ ./SNANDer -d -v -w ecc_2Gb_2K_64_flashimage_rfb1_ac2600.bin

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.4 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
spi_nand_probe: mfr_id = 0xc2, dev_id = 0x22
Get Status Register 1: 0x00
Get Status Register 2: 0x11
Disable Flash ECC.
Detected SPI NAND Flash: MXIC MX35LF2G, Flash Size: 256 MB
WRITE:
Write addr = 0x0000000000000000, len = 0x000000000E5A9D6F
Written 100% [240819567] of [240819567] bytes
Elapsed time: 4184 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x000000000E5A9D6F
Read 100% [240819567] of [240819567] bytes
Elapsed time: 2047 seconds
Status: OK
$
```

4. Read Microwire EEPROM Atmel 93C46 and save file.

```
$ ./SNANDer -E 93c46 -r test.bin

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.4 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
Microwire EEPROM chip: 93c46, Size: 64 bytes, Org: 16 bits, fix addr len: Auto
READ:
Read addr = 0x0000000000000000, len = 0x0000000000000080
Read_EEPROM_3wire: Set address len 6 bits
Read 100% [64] of [64] bytes
Read [128] bytes from [93c46] EEPROM address 0x00000000
Elapsed time: 0 seconds
Status: OK
```

5. Write and verify Microwire EEPROM Atmel 93C46 from file.

```
$ ./SNANDer -E 93c46 -w test.bin -v

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.4 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
Microwire EEPROM chip: 93c46, Size: 64 bytes, Org: 16 bits, fix addr len: Auto
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000000000080
Erase_EEPROM_3wire: Set address len 6 bits
Write_EEPROM_3wire: Set address len 6 bits
Written 100% [64] of [64] bytes
Wrote [128] bytes to [93c46] EEPROM address 0x00000000
Elapsed time: 1 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000000000080
Read_EEPROM_3wire: Set address len 6 bits
Read 100% [64] of [64] bytes
Read [128] bytes from [93c46] EEPROM address 0x00000000
Elapsed time: 1 seconds
Status: OK
```

**Supported Chips**

```
SPI NAND Flash Support List:
001. GIGADEVICE GD5F1GQ4UA
002. GIGADEVICE GD5F1GQ4UB
003. GIGADEVICE GD5F1GQ4UC
004. GIGADEVICE GD5F1GQ4UE
005. GIGADEVICE GD5F1GQ5UE
006. GIGADEVICE GD5F1GQ5RE
007. GIGADEVICE GD5F2GQ4UB
008. GIGADEVICE GD5F2GQ4UE
009. GIGADEVICE GD5F2GQ4UC
010. GIGADEVICE GD5F4GQ4UB
011. GIGADEVICE GD5F4GQ4UC
012. ESMT F50L512
013. ESMT F50L1G
014. ESMT F50L1G41LB
015. ESMT F50L2G41LB
016. WINBOND W25N01G
017. WINBOND W25M02G
018. MXIC MX35LF1G
019. MXIC MX35LF2G
020. ZENTEL A5U12A21ASC
021. ZENTEL A5U1GA21BWS
022. ETRON EM73C044SNB
023. ETRON EM73C044SND
024. ETRON EM73C044SNF
025. ETRON EM73C044VCA
026. ETRON EM73C044VCD
027. ETRON EM73D044VCA
028. ETRON EM73D044VCB
029. ETRON EM73D044VCD
030. ETRON EM73D044VCG
031. ETRON EM73D044VCH
032. ETRON EM73D044SNA
033. ETRON EM73D044SNC
034. ETRON EM73D044SND
035. ETRON EM73D044SNF
036. ETRON EM73E044SNA
037. TOSHIBA TC58CVG0S3H
038. TOSHIBA TC58CVG1S3H
039. TOSHIBA TC58CVG2S0H
040. KIOXIA TC58CVG2S0HRAIJ
041. MICRON MT29F1G01
042. MICRON MT29F2G01
043. MICRON MT29F4G01
044. HEYANG HYF1GQ4UAACAE
045. HEYANG HYF2GQ4UAACAE
046. HEYANG HYF2GQ4UHCCAE
047. HEYANG HYF1GQ4UDACAE
048. HEYANG HYF2GQ4UDACAE
049. PN PN26G01A-X
050. PN PN26G02A-X
051. PN PN26Q01A-X
052. ATO ATO25D1GA
053. ATO ATO25D2GA
054. ATO ATO25D2GB
055. FM FM25S01
056. FM FM25S01A
057. FM FM25G01B
058. FM FM25G02B
059. FM FM25G02C
060. FM FM25G02
061. XTX XT26G02B
062. XTX XT26G01A
063. XTX XT26G02A
064. MIRA PSU1GS20BN
065. BIWIN BWJX08U
066. BIWIN BWET08U
067. FORESEE FS35ND01GD1F1
068. FORESEE FS35ND01GS1F1
069. FORESEE FS35ND02GS2F1
070. FORESEE FS35ND02GD1F1
071. DS DS35Q2GA
072. DS DS35Q1GA
073. FISON CS11G0T0A0AA
074. FISON CS11G1T0A0AA
075. FISON CS11G0G0A0AA
076. TYM TYM25D2GA01
077. TYM TYM25D2GA02
078. TYM TYM25D1GA03

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
055. W25Q16JQ
056. W25Q16JM
057. W25Q32BV
058. W25Q32DW
059. W25Q64BV
060. W25Q64DW
061. W25Q128BV
062. W25Q128FW
063. W25Q256FV
064. W25Q512JV
065. M25P016
066. N25Q032A
067. N25Q064A
068. M25P128
069. N25Q128A
070. XM25QH32B
071. XM25QH32A
072. XM25QH64A
073. XM25QH128A
074. N25Q256A
075. MT25QL512AB
076. ZB25VQ16
077. ZB25VQ32
078. ZB25VQ64
079. ZB25VQ128
080. BH25D80
081. BY25Q16BS
082. BY25Q32BS
083. BY25Q64AS
084. BY25Q128AS
085. XT25F32B
086. XT25F32B
087. XT25F64B
088. XT25F128B
089. PM25LQ016
090. PM25LQ032
091. PM25LQ064
092. PM25LQ128
093. IC25LP016
094. IC25LP032
095. IC25LP064
096. IC25LP128
097. FS25Q016
098. FS25Q032
099. FS25Q064
100. FS25Q128
101. FM25W16
102. FM25W32
103. FM25W64
104. FM25W128
105. FM25Q16A
106. FM25Q32A
107. FM25Q64A
108. FM25Q128A
109. PN25F16
110. PN25F32
111. PN25F64
112. PN25F128
113. P25Q16H
114. P25Q32H
115. P25Q64H
116. P25Q128H

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
