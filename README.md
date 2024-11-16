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
002. GIGADEVICE GD5F2GQ4UA
003. GIGADEVICE GD5F1GQ4UB
004. GIGADEVICE GD5F1GQ4RE
005. GIGADEVICE GD5F1GQ4UC
006. GIGADEVICE GD5F1GQ4UE
007. GIGADEVICE GD5F1GQ5UE
008. GIGADEVICE GD5F1GQ5RE
009. GIGADEVICE GD5F2GQ5UE
010. GIGADEVICE GD5F2GQ5RE
011. GIGADEVICE GD5F1GM7UE
012. GIGADEVICE GD5F1GM7RE
013. GIGADEVICE GD5F2GM7UE
014. GIGADEVICE GD5F2GM7RE
015. GIGADEVICE GD5F2GQ4UB
016. GIGADEVICE GD5F2GQ4UE
017. GIGADEVICE GD5F2GQ4UC
018. GIGADEVICE GD5F4GQ4UB
019. GIGADEVICE GD5F4GQ4UC
020. ESMT F50D1G41LB(2M)
021. ESMT F50L512
022. ESMT F50L1G
023. ESMT F50L1G41LB
024. ESMT F50L2G41LB
025. WINBOND W25N01GV
026. WINBOND W25N01GW
027. WINBOND W25N02KV
028. WINBOND W25N04KV
029. WINBOND W25M02GV
030. MXIC MX35LF1GE4AB
031. MXIC MX35LF2GE4AB
032. MXIC MX35LF2G14AC
033. MXIC MX35UF1G14AC
034. MXIC MX35UF2G14AC
035. MXIC MX35LF2GE4AD
036. MXIC MX35LF1G24AD
037. MXIC MX35LF2G24AD
038. MXIC MX35LF4G24AD
039. MXIC MX35LF2G24AD-Z4I8
040. MXIC MX35LF4G24AD-Z4I8
041. MXIC MX35UF1GE4AD
042. MXIC MX35UF2GE4AD
043. MXIC MX35UF4GE4AD
044. ZENTEL A5U12A21ASC
045. ZENTEL A5U1GA21BWS
046. ETRON EM73C044SNB
047. ETRON EM73C044SND
048. ETRON EM73C044SNF
049. ETRON EM73C044VCA
050. ETRON EM73C044VCD
051. ETRON EM73D044VCA
052. ETRON EM73D044VCB
053. ETRON EM73D044VCD
054. ETRON EM73D044VCG
055. ETRON EM73D044VCH
056. ETRON EM73D044SNA
057. ETRON EM73D044SNC
058. ETRON EM73D044SND
059. ETRON EM73D044SNF
060. ETRON EM73E044SNA
061. TOSHIBA TC58CVG0S3H
062. TOSHIBA TC58CVG1S3H
063. TOSHIBA TC58CVG2S0H
064. KIOXIA TC58CVG2S0HRAIJ
065. MICRON MT29F1G01AA
066. MICRON MT29F2G01AA
067. MICRON MT29F4G01AA
068. MICRON MT29F1G01AB
069. MICRON MT29F2G01ABA
070. MICRON MT29F2G01ABB
071. MICRON MT29F4G01AB
072. MICRON MT29F4G01AD
073. HEYANG HYF1GQ4UAACAE
074. HEYANG HYF2GQ4UAACAE
075. HEYANG HYF2GQ4UHCCAE
076. HEYANG HYF1GQ4UDACAE
077. HEYANG HYF2GQ4UDACAE
078. HEYANG HYF2GQ4UTACAE
079. PN PN26G01A-X
080. PN PN26G02A-X
081. PN PN26Q01A-X
082. ATO ATO25D1GA
083. ATO ATO25D2GA
084. ATO ATO25D2GB
085. FM FM25S01
086. FM FM25S01A
087. FM FM25S02A
088. FM FM25G01B
089. FM FM25G02B
090. FM FM25G02C
091. FM FM25G02
092. XTX XT26G02B
093. XTX XT26G01C
094. XTX XT26G02C
095. XTX XT26G01A
096. XTX XT26G02A
097. MIRA PSU1GS20BN
098. BIWIN BWJX08U
099. BIWIN BWET08U
100. FORESEE FS35ND01GD1F1
101. FORESEE FS35ND01GS1F1
102. FORESEE FS35ND02GS2F1
103. FORESEE FS35ND02GD1F1
104. FORESEE FS35ND01GS1Y2
105. FORESEE FS35ND02G-S3Y2
106. FORESEE FS35ND04G-S2Y2
107. DS DS35Q2GA
108. DS DS35M2GA
109. DS DS35Q1GA
110. DS DS35M1GA
111. FISON CS11G0T0A0AA
112. FISON CS11G1T0A0AA
113. FISON CS11G0G0A0AA
114. TYM TYM25D2GA01
115. TYM TYM25D2GA02
116. TYM TYM25D1GA03
117. XINCUN XCSP1AAWH-NT

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
012. ES25P10
013. ES25P20
014. ES25P40
015. ES25P80
016. ES25P16
017. ES25P32
018. ES25M40A
019. ES25M80A
020. ES25M16A
021. DQ25Q64AS
022. DQ25Q128AL
023. F25L016
024. F25L16QA
025. F25L032
026. F25L32QA
027. F25L064
028. F25L64QA
029. GD25Q20C
030. GD25Q40C
031. GD25Q80C
032. GD25LQ80C
033. GD25WD80C
034. GD25WQ80E
035. GD25Q16
036. GD25LQ16C
037. GD25WQ16E
038. GD25Q32
039. GD25LQ32E
040. GD25WQ32E
041. GD25Q64CSIG
042. GD25LQ64E
043. GD25Q128CSIG
044. GD25LQ128D
045. GD25F256F
046. GD25Q256CSIG
047. MX25L4005A
048. MX25L8005M
049. MX25L1605D
050. MX25U1635F
051. MX25L3205D
052. MX25U3235F
053. MX25L6405D
054. MX25U6435F
055. MX25L12805D
056. MX25U12835F
057. MX25L25635E
058. MX25U25643G
059. MX25L51245G
060. MX25U51245G
061. YC25Q128
062. FL016AIF
063. FL064AIF
064. S25FL016P
065. S25FL032P
066. S25FL064P
067. S25FL128P
068. S25FL129P
069. S25FL256S
070. S25FL512S
071. S25FL116K
072. S25FL132K
073. S25FL164K
074. EN25F16
075. EN25Q16
076. EN25QH16
077. EN25Q32B
078. EN25F32
079. EN25F64
080. EN25Q64
081. EN25QA64A
082. EN25QH64A
083. EN25Q128
084. EN25Q256
085. EN25QA128A
086. EN25QH128A
087. GM25Q128A
088. W25X05
089. W25X10
090. W25X20
091. W25X40
092. W25X80
093. W25X16
094. W25X32VS
095. W25X64
096. W25Q20CL
097. W25Q20BW
098. W25Q20EW
099. W25Q80
100. W25Q80BL
101. W25Q16JQ
102. W25Q16JM
103. W25Q32BV
104. W25Q32DW
105. W25Q32JWIM
106. W25Q64BV
107. W25Q64DW
108. W25Q64JVIM
109. W25Q64JWIM
110. W25Q128BV
111. W25Q128FW
112. W25Q256FV
113. W25Q256JW
114. W25Q256JWIM
115. W25Q512JV
116. W25Q512JVIM
117. W25Q512NW
118. W25Q512NWIM
119. M25P05
120. M25P10
121. M25P20
122. M25P40
123. M25P80
124. M25P16
125. M25P32
126. M25P64
127. M25P128
128. N25Q016A
129. N25Q032A
130. N25Q032A
131. N25Q064A
132. N25Q064A
133. N25Q128A
134. N25Q128A
135. N25Q256A
136. N25Q512A
137. MT25QL64AB
138. MT25QU64AB
139. MT25QL128AB
140. MT25QU128AB
141. MT25QL256AB
142. MT25QU256AB
143. MT25QL512AB
144. MT25QU512AB
145. XM25QH10B
146. XM25QH20B
147. XM25QU41B
148. XM25QH40B
149. XM25QU80B
150. XM25QH80B
151. XM25QU16B
152. XM25QH16C
153. XM25QW16C
154. XM25QH32B
155. XM25QW32C
156. XM25LU32C
157. XM25QH32A
158. XM25QH64C
159. XM25LU64C
160. XM25QW64C
161. XM25QH64A
162. XM25QH128A
163. XM25QH128C
164. XM25LU128C
165. XM25QW128C
166. XM25QH256C
167. XM25QU256C
168. XM25QW256C
169. XM25QH512C
170. XM25QU512C
171. XM25QW512C
172. MD25D20
173. MD25D40
174. ZB25VQ16
175. ZB25LQ16
176. ZB25VQ32
177. ZB25LQ32
178. ZB25VQ64
179. ZB25LQ64
180. ZB25VQ128
181. ZB25LQ128
182. LE25U20AMB
183. LE25U40CMC
184. BY25D05AS
185. BY25D10AS
186. BY25D20AS
187. BY25D40AS
188. BY25Q40BL
189. BY25Q40BL
190. BY25Q80BS
191. BY25Q16BS
192. BY25Q16BL
193. BY25Q32BS
194. BY25Q32AL
195. BY25Q64AS
196. BY25Q64AL
197. BY25Q128AS
198. BY25Q128EL
199. BY25Q256ES
200. XT25F04D
201. XT25F08B
202. XT25F08D
203. XT25F16B
204. XT25Q16D
205. XT25F32B
206. XT25F64B
207. XT25Q64D
208. XT25F128B
209. XT25F128D
210. PM25LQ016
211. PM25LQ032
212. PM25LQ064
213. PM25LQ128
214. IS25LQ010
215. IS25LQ020
216. IS25WP040D
217. IS25LP080D
218. IS25WP080D
219. IS25LP016D
220. IS25WP016D
221. IS25LP032D
222. IS25WP032D
223. IS25LP064D
224. IS25WP064D
225. IS25LP128F
226. IS25WP128F
227. IS25LP256D
228. IS25WP256D
229. IS25LP256D
230. IS25WP256D
231. FM25W04
232. FM25Q04
233. FM25Q08
234. FM25W16
235. FM25Q16
236. FM25W32
237. FS25Q32
238. FM25W64
239. FS25Q64
240. FM25W128
241. FS25Q128
242. FM25Q04A
243. FM25M04A
244. FM25Q08A
245. FM25M08A
246. FM25Q16A
247. FM25M16A
248. FM25Q32A
249. FM25M32B
250. FM25Q64A
251. FM25M64A
252. FM25Q128A
253. PN25F16
254. PN25F32
255. PN25F64
256. PN25F128
257. P25D05H
258. P25D10H
259. P25D20H
260. P25D40H
261. P25D80H
262. P25Q16H
263. P25Q32H
264. P25Q64H
265. P25Q128H
266. PY25Q128HA
267. SK25P32
268. SK25P64
269. SK25P128
270. ZD25Q16A
271. ZD25Q32A
272. ZD25Q64A
273. ZD25Q128A
274. ZD25Q16B
275. ZD25Q32B
276. ZD25Q64B
277. ZD25Q128B
278. PCT25VF010A
279. PCT25VF020B
280. PCT25VF040B
281. PCT25VF080B
282. PCT25VF016B
283. PCT25VF032B
284. PCT25VF064C
285. PCT26VF016
286. PCT26VF032

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
