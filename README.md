SNANDer
===

SNANDer - Serial Nor/nAND/Eeprom programmeR (based on CH341A)

![ch341a-nextProgrammer](https://raw.githubusercontent.com/Droid-MAX/SNANDer/master/photos/ch341a-nextProgrammer.jpg)

**Requirements**

* gcc/mingw-w64, wget, make, and libusb-1.0-dev (via apt Command Line Tools)

**Compiling for Linux**

After installing the gcc and necessary tools, building `snander` is as simple as running the command:

```
make -C src/
```

After successful compilation, the target executable file will be generated in `src` folder.

Or you can choose static compile, building `snander` statically is as simple as running the command:

```
./build-for-linux.sh
```

After successful compilation, the target executable file will be generated in the `build` folder.

**Compiling for Windows**

After installing the mingw-w64 and necessary tools, building `snander` is as simple as running the included script:

```
./build-for-windows.sh
```

After successful compilation, the target executable file will be generated in the `build` folder, include x86 and x64 binaries.

**Compiling for MacOS**

After installing the necessary tools, building `snander` is as simple as running the included script:

```
./build-for-darwin.sh
```

After successful compilation, the target executable file will be generated in the `build` folder.

**Compiling for OpenWrt IPK**

First download `OpenWrt SDK` and extract it

```
    cd /path/to/your/sdk
    git clone https://github.com/Droid-MAX/SNANDer package/snander
    make menuconfig # Choose `snander` in section `Utilities`
    make package/snander/compile V=s
```

**Usage**

Using `snander` is straightforward:

```
SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

  Usage:
 -h             display this message
 -d             disable internal ECC(use read and write page size + OOB size)
 -o <bytes>     manual set OOB size with disable internal ECC(default 0)
 -I             ECC ignore errors(for read test only)
 -k             Skip BAD pages, try to read or write in next page
 -L             print list support chips
 -i             read the chip ID info
 -E             select I2C EEPROM {24c01|24c02|24c04|24c08|24c16|24c32|24c64|24c128|24c256|24c512|24c1024}
                select Microwire EEPROM {93c06|93c16|93c46|93c56|93c66|93c76|93c86|93c96} (need SPI-to-MW adapter)
                select SPI EEPROM 25xxx {25010|25020|25040|25080|25160|25320|25640|25128|25256|25512|251024}
 -8             set organization 8-bit for Microwire EEPROM(default 16-bit) and set jumper on SPI-to-MW adapter
 -f <addr len>  set manual address size in bits for Microwire EEPROM(default auto)
 -s <bytes>     set page size from datasheet for fast write SPI EEPROM(default not use)
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

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

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

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

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

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

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

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

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

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

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

6. Write and verify SPI EEPROM Atmel AT25640B from file.

```
$ ./SNANDer -E 25640 -v -w test.bin

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc_at_mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
SPI EEPROM chip: 25640, Size: 8192 bytes
WRITE:
Written addr = 0x0000000000000000, len = 0x0000000000002000
Wrote 100% [8192] bytes to [25640] EEPROM address 0x00000000
Elapsed time: 22 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000000002000
Read 100% [8192] bytes from [25640] EEPROM address 0x00000000
Elapsed time: 2 seconds
Status: OK
```

7. Fast write and verify SPI EEPROM Atmel AT25640B from file with use page size.
   (Find out page size from datasheet on chip!!!)

```
$ ./SNANDer -E 25640 -v -w test.bin -s 32

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc_at_mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
SPI EEPROM chip: 25640, Size: 8192 bytes
Setting page size 32B for write.
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000000002000
Written 100% [8192] bytes to [25640] EEPROM address 0x00000000
Elapsed time: 1 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000000002000
Read 100% [8192] bytes from [25640] EEPROM address 0x00000000
Elapsed time: 2 seconds
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
012. ESMT F50D1G41LB(2M)
013. ESMT F50L512
014. ESMT F50L1G
015. ESMT F50L1G41LB
016. ESMT F50L2G41LB
017. WINBOND W25N01G
018. WINBOND W25N02KV
019. WINBOND W25M02G
020. MXIC MX35LF1G
021. MXIC MX35LF2G
022. MXIC MX35LF2GE4AD
023. ZENTEL A5U12A21ASC
024. ZENTEL A5U1GA21BWS
025. ETRON EM73C044SNB
026. ETRON EM73C044SND
027. ETRON EM73C044SNF
028. ETRON EM73C044VCA
029. ETRON EM73C044VCD
030. ETRON EM73D044VCA
031. ETRON EM73D044VCB
032. ETRON EM73D044VCD
033. ETRON EM73D044VCG
034. ETRON EM73D044VCH
035. ETRON EM73D044SNA
036. ETRON EM73D044SNC
037. ETRON EM73D044SND
038. ETRON EM73D044SNF
039. ETRON EM73E044SNA
040. TOSHIBA TC58CVG0S3H
041. TOSHIBA TC58CVG1S3H
042. TOSHIBA TC58CVG2S0H
043. KIOXIA TC58CVG2S0HRAIJ
044. MICRON MT29F1G01
045. MICRON MT29F2G01
046. MICRON MT29F4G01
047. HEYANG HYF1GQ4UAACAE
048. HEYANG HYF2GQ4UAACAE
049. HEYANG HYF2GQ4UHCCAE
050. HEYANG HYF1GQ4UDACAE
051. HEYANG HYF2GQ4UDACAE
052. PN PN26G01A-X
053. PN PN26G02A-X
054. PN PN26Q01A-X
055. ATO ATO25D1GA
056. ATO ATO25D2GA
057. ATO ATO25D2GB
058. FM FM25S01
059. FM FM25S01A
060. FM FM25G01B
061. FM FM25G02B
062. FM FM25G02C
063. FM FM25G02
064. XTX XT26G02B
065. XTX XT26G01C
066. XTX XT26G01A
067. XTX XT26G02A
068. MIRA PSU1GS20BN
069. BIWIN BWJX08U
070. BIWIN BWET08U
071. FORESEE FS35ND01GD1F1
072. FORESEE FS35ND01GS1F1
073. FORESEE FS35ND02GS2F1
074. FORESEE FS35ND02GD1F1
075. FORESEE FS35ND01GS1Y2
076. FORESEE FS35ND02G-S3Y2
077. FORESEE FS35ND04G-S2Y2
078. DS DS35Q2GA
079. DS DS35Q1GA
080. FISON CS11G0T0A0AA
081. FISON CS11G1T0A0AA
082. FISON CS11G0G0A0AA
083. TYM TYM25D2GA01
084. TYM TYM25D2GA02
085. TYM TYM25D1GA03
086. XINCUN XCSP1AAWH-NT

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
013. YC25Q128
014. GD25Q256CSIG
015. MX25L8005M
016. MX25L1605D
017. MX25L3205D
018. MX25L6405D
019. MX25L12805D
020. MX25L25635E
021. MX25L51245G
022. FL016AIF
023. FL064AIF
024. S25FL016P
025. S25FL032P
026. S25FL064P
027. S25FL128P
028. S25FL129P
029. S25FL256S
030. S25FL512S
031. S25FL116K
032. S25FL132K
033. S25FL164K
034. EN25F16
035. EN25Q16
036. EN25QH16
037. EN25Q32B
038. EN25F32
039. EN25F64
040. EN25Q64
041. EN25QA64A
042. EN25QH64A
043. EN25Q128
044. EN25Q256
045. EN25QA128A
046. EN25QH128A
047. GM25Q128A
048. W25X05
049. W25X10
050. W25X20
051. W25X40
052. W25X80
053. W25X16
054. W25X32VS
055. W25X64
056. W25Q20CL
057. W25Q20BW
058. W25Q20EW
059. W25Q80
060. W25Q80BL
061. W25Q16JQ
062. W25Q16JM
063. W25Q32BV
064. W25Q32DW
065. W25Q64BV
066. W25Q64DW
067. W25Q128BV
068. W25Q128FW
069. W25Q256FV
070. W25Q256JW
071. W25Q256JWIM
072. W25Q512JV
073. W25Q512JVIM
074. W25Q512NW
075. W25Q512NWIM
076. M25P016
077. N25Q032A
078. N25Q064A
079. M25P128
080. N25Q128A
081. XM25QH32B
082. XM25QH32A
083. XM25QH64A
084. XM25QH128A
085. N25Q256A
086. MT25QL512AB
087. MD25D20
088. MD25D40
089. ZB25VQ16
090. ZB25VQ32
091. ZB25VQ64
092. ZB25VQ128
093. BY25D10
094. BY25D20
095. BY25D40
096. BY25D80
097. BY25Q16BS
098. BY25Q32BS
099. BY25Q64AS
100. BY25Q128AS
101. XT25F32B
102. XT25F32B
103. XT25F64B
104. XT25F128B
105. XT25Q128D
106. PM25LQ016
107. PM25LQ032
108. PM25LQ064
109. PM25LQ128
110. IC25LP016
111. IC25LP032
112. IC25LP064
113. IC25LP128
114. FM25Q16
115. FM25Q32
116. FM25Q64
117. FM25Q128
118. FM25W16
119. FM25W32
120. FM25W64
121. FM25W128
122. FM25Q16A
123. FM25Q32A
124. FM25Q64A
125. FM25Q128A
126. PN25F16
127. PN25F32
128. PN25F64
129. PN25F128
130. P25Q16H
131. P25Q32H
132. P25Q64H
133. P25Q128H
134. SK25P32
135. SK25P64
136. SK25P128

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

SPI EEPROM Support List:
001. 25010
002. 25020
003. 25040
004. 25080
005. 25160
006. 25320
007. 25640
008. 25128
009. 25256
010. 25512
011. 251024
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
