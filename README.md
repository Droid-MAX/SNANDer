SNANDer
===

SNANDer - Serial Nor/nAND/Eeprom programmeR (based on CH341A)

![ch341a-nextProgrammer](photos/ch341a-nextProgrammer.jpg)

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
spi device id: ff c2 22 c2 22 (c222c222)
SPI NOR Flash Not Detected!
spi_nand_probe: mfr_id = 0xc2, dev_id = 0x22
Get Status Register 1: 0x38
Get Status Register 2: 0x10
Using Flash ECC.
Detected SPI NAND Flash: MXIC MX35LF2GE4AB, Flash Size: 256 MB
$
```

2. Full erase flash with disable internal ECC check. Without OOB, page size 2112 bytes.

```
$ ./SNANDer -d -e

SNANDer - Serial Nor/nAND/Eeprom programmeR v.1.7.8 by McMCC <mcmcc@mail.ru>

Found programmer device: WinChipHead (WCH) - CH341A
Device revision is 3.0.4
spi device id: ff c2 22 c2 22 (c222c222)
SPI NOR Flash Not Detected!
spi_nand_probe: mfr_id = 0xc2, dev_id = 0x22
Get Status Register 1: 0x00
Get Status Register 2: 0x11
Disable Flash ECC.
Detected SPI NAND Flash: MXIC MX35LF2GE4AB, Flash Size: 256 MB
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
spi device id: ff c2 22 c2 22 (c222c222)
SPI NOR Flash Not Detected!
spi_nand_probe: mfr_id = 0xc2, dev_id = 0x22
Get Status Register 1: 0x00
Get Status Register 2: 0x11
Disable Flash ECC.
Detected SPI NAND Flash: MXIC MX35LF2GE4AB, Flash Size: 256 MB
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
017. WINBOND W25N01GV
018. WINBOND W25N01GW
019. WINBOND W25N02KV
020. WINBOND W25M02GV
021. MXIC MX35LF1GE4AB
022. MXIC MX35LF2GE4AB
023. MXIC MX35LF2G14AC
024. MXIC MX35LF2GE4AD
025. ZENTEL A5U12A21ASC
026. ZENTEL A5U1GA21BWS
027. ETRON EM73C044SNB
028. ETRON EM73C044SND
029. ETRON EM73C044SNF
030. ETRON EM73C044VCA
031. ETRON EM73C044VCD
032. ETRON EM73D044VCA
033. ETRON EM73D044VCB
034. ETRON EM73D044VCD
035. ETRON EM73D044VCG
036. ETRON EM73D044VCH
037. ETRON EM73D044SNA
038. ETRON EM73D044SNC
039. ETRON EM73D044SND
040. ETRON EM73D044SNF
041. ETRON EM73E044SNA
042. TOSHIBA TC58CVG0S3H
043. TOSHIBA TC58CVG1S3H
044. TOSHIBA TC58CVG2S0H
045. KIOXIA TC58CVG2S0HRAIJ
046. MICRON MT29F1G01
047. MICRON MT29F2G01
048. MICRON MT29F4G01
049. HEYANG HYF1GQ4UAACAE
050. HEYANG HYF2GQ4UAACAE
051. HEYANG HYF2GQ4UHCCAE
052. HEYANG HYF1GQ4UDACAE
053. HEYANG HYF2GQ4UDACAE
054. PN PN26G01A-X
055. PN PN26G02A-X
056. PN PN26Q01A-X
057. ATO ATO25D1GA
058. ATO ATO25D2GA
059. ATO ATO25D2GB
060. FM FM25S01
061. FM FM25S01A
062. FM FM25G01B
063. FM FM25G02B
064. FM FM25G02C
065. FM FM25G02
066. XTX XT26G02B
067. XTX XT26G01C
068. XTX XT26G01A
069. XTX XT26G02A
070. MIRA PSU1GS20BN
071. BIWIN BWJX08U
072. BIWIN BWET08U
073. FORESEE FS35ND01GD1F1
074. FORESEE FS35ND01GS1F1
075. FORESEE FS35ND02GS2F1
076. FORESEE FS35ND02GD1F1
077. FORESEE FS35ND01GS1Y2
078. FORESEE FS35ND02G-S3Y2
079. FORESEE FS35ND04G-S2Y2
080. DS DS35Q2GA
081. DS DS35M2GA
082. DS DS35Q1GA
083. DS DS35M1GA
084. FISON CS11G0T0A0AA
085. FISON CS11G1T0A0AA
086. FISON CS11G0G0A0AA
087. TYM TYM25D2GA01
088. TYM TYM25D2GA02
089. TYM TYM25D1GA03
090. XINCUN XCSP1AAWH-NT

SPI NOR Flash Support List:
001. AT26DF161
002. AT25DF321
003. A25L10PU
004. A25L20PU
005. A25L040
006. A25LQ080
007. A25L080
008. A25LQ16
009. A25LQ32
010. A25L032
011. A25LQ64
012. DQ25Q64AS
013. F25L016
014. F25L16QA
015. F25L032
016. F25L32QA
017. F25L064
018. F25L64QA
019. GD25Q20C
020. GD25Q40C
021. GD25Q80C
022. GD25LQ80C
023. GD25WD80C
024. GD25WQ80E
025. GD25Q16
026. GD25LQ16C
027. GD25WQ16E
028. GD25Q32
029. GD25LQ32E
030. GD25WQ32E
031. GD25Q64CSIG
032. GD25Q128CSIG
033. GD25Q256CSIG
034. MX25L8005M
035. MX25L1605D
036. MX25L3205D
037. MX25L6405D
038. MX25L12805D
039. MX25L25635E
040. MX25L51245G
041. YC25Q128
042. FL016AIF
043. FL064AIF
044. S25FL016P
045. S25FL032P
046. S25FL064P
047. S25FL128P
048. S25FL129P
049. S25FL256S
050. S25FL512S
051. S25FL116K
052. S25FL132K
053. S25FL164K
054. EN25F16
055. EN25Q16
056. EN25QH16
057. EN25Q32B
058. EN25F32
059. EN25F64
060. EN25Q64
061. EN25QA64A
062. EN25QH64A
063. EN25Q128
064. EN25Q256
065. EN25QA128A
066. EN25QH128A
067. GM25Q128A
068. W25X05
069. W25X10
070. W25X20
071. W25X40
072. W25X80
073. W25X16
074. W25X32VS
075. W25X64
076. W25Q20CL
077. W25Q20BW
078. W25Q20EW
079. W25Q80
080. W25Q80BL
081. W25Q16JQ
082. W25Q16JM
083. W25Q32BV
084. W25Q32DW
085. W25Q32JWIM
086. W25Q64BV
087. W25Q64DW
088. W25Q128BV
089. W25Q128FW
090. W25Q256FV
091. W25Q256JW
092. W25Q256JWIM
093. W25Q512JV
094. W25Q512JVIM
095. W25Q512NW
096. W25Q512NWIM
097. M25P016
098. M25P128
099. N25Q032A
100. N25Q032A
101. N25Q064A
102. N25Q064A
103. N25Q128A
104. N25Q128A
105. N25Q256A
106. MT25QL64AB
107. MT25QU64AB
108. MT25QL128AB
109. MT25QU128AB
110. MT25QL256AB
111. MT25QU256AB
112. MT25QL512AB
113. MT25QU512AB
114. XM25QH10B
115. XM25QH20B
116. XM25QU41B
117. XM25QH40B
118. XM25QU80B
119. XM25QH80B
120. XM25QU16B
121. XM25QH16C
122. XM25QW16C
123. XM25QH32B
124. XM25QW32C
125. XM25LU32C
126. XM25QH32A
127. XM25QH64C
128. XM25LU64C
129. XM25QW64C
130. XM25QH64A
131. XM25QH128A
132. XM25QH128C
133. XM25LU128C
134. XM25QW128C
135. XM25QH256C
136. XM25QU256C
137. XM25QW256C
138. XM25QH512C
139. XM25QU512C
140. XM25QW512C
141. MD25D20
142. MD25D40
143. NM25Q32E
144. NM25Q64E
145. NM25Q128E
146. NM25L256F
147. ZB25VQ16
148. ZB25LQ16
149. ZB25VQ32
150. ZB25LQ32
151. ZB25VQ64
152. ZB25LQ64
153. ZB25VQ128
154. ZB25LQ128
155. BY25D05AS
156. BY25D10AS
157. BY25D20AS
158. BY25D40AS
159. BY25Q40BL
160. BY25Q40BL
161. BY25Q80BS
162. BY25Q16BS
163. BY25Q16BL
164. BY25Q32BS
165. BY25Q32AL
166. BY25Q64AS
167. BY25Q64AL
168. BY25Q128AS
169. BY25Q128EL
170. BY25Q256ES
171. XT25F04D
172. XT25F08B
173. XT25F08D
174. XT25F16B
175. XT25Q16D
176. XT25F32B
177. XT25F64B
178. XT25Q64D
179. XT25F128B
180. XT25F128D
181. PM25LQ016
182. PM25LQ032
183. PM25LQ064
184. PM25LQ128
185. IS25LQ010
186. IS25LQ020
187. IS25WP040D
188. IS25LP080D
189. IS25WP080D
190. IS25LP016D
191. IS25WP016D
192. IS25LP032D
193. IS25WP032D
194. IS25LP064D
195. IS25WP064D
196. IS25LP128F
197. IS25WP128F
198. IS25LP256D
199. IS25WP256D
200. IS25LP256D
201. IS25WP256D
202. FM25W04
203. FM25Q04
204. FM25Q08
205. FM25W16
206. FM25Q16
207. FM25W32
208. FS25Q32
209. FM25W64
210. FS25Q64
211. FM25W128
212. FS25Q128
213. FM25Q04A
214. FM25M04A
215. FM25Q08A
216. FM25M08A
217. FM25Q16A
218. FM25M16A
219. FM25Q32A
220. FM25M32B
221. FM25Q64A
222. FM25M64A
223. FM25Q128A
224. PN25F16
225. PN25F32
226. PN25F64
227. PN25F128
228. P25D05H
229. P25D10H
230. P25D20H
231. P25D40H
232. P25D80H
233. P25Q16H
234. P25Q32H
235. P25Q64H
236. P25Q128H
237. PY25Q128HA
238. SK25P32
239. SK25P64
240. SK25P128
241. ZD25Q16B
242. ZD25Q32C
243. ZD25LQ64
244. ZD25Q64B
245. ZD25LQ128

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
