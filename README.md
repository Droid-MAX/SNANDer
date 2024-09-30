SNANDer
===

SNANDer - Serial Nor/nAND/Eeprom programmeR (based on CH347)

![ch347-nextProgrammer](photos/ch347-nextProgrammer.jpg)

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

Open CH347 device success.
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

Open CH347 device success.
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

Open CH347 device success.
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

Open CH347 device success.
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

Open CH347 device success.
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

Open CH347 device success.
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

Open CH347 device success.
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
004. GIGADEVICE GD5F1GQ4UC
005. GIGADEVICE GD5F1GQ4UE
006. GIGADEVICE GD5F1GQ5UE
007. GIGADEVICE GD5F1GQ5RE
008. GIGADEVICE GD5F2GQ5UE
009. GIGADEVICE GD5F2GQ5RE
010. GIGADEVICE GD5F2GQ4UB
011. GIGADEVICE GD5F2GQ4UE
012. GIGADEVICE GD5F2GQ4UC
013. GIGADEVICE GD5F4GQ4UB
014. GIGADEVICE GD5F4GQ4UC
015. ESMT F50D1G41LB(2M)
016. ESMT F50L512
017. ESMT F50L1G
018. ESMT F50L1G41LB
019. ESMT F50L2G41LB
020. WINBOND W25N01GV
021. WINBOND W25N01GW
022. WINBOND W25N02KV
023. WINBOND W25N04KV
024. WINBOND W25M02GV
025. MXIC MX35LF1GE4AB
026. MXIC MX35LF2GE4AB
027. MXIC MX35LF2G14AC
028. MXIC MX35LF2GE4AD
029. MXIC MX35LF1G24AD
030. MXIC MX35LF2G24AD
031. MXIC MX35LF4G24AD
032. MXIC MX35LF2G24AD-Z4I8
033. MXIC MX35LF4G24AD-Z4I8
034. MXIC MX35UF1GE4AD
035. MXIC MX35UF2GE4AD
036. MXIC MX35UF4GE4AD
037. ZENTEL A5U12A21ASC
038. ZENTEL A5U1GA21BWS
039. ETRON EM73C044SNB
040. ETRON EM73C044SND
041. ETRON EM73C044SNF
042. ETRON EM73C044VCA
043. ETRON EM73C044VCD
044. ETRON EM73D044VCA
045. ETRON EM73D044VCB
046. ETRON EM73D044VCD
047. ETRON EM73D044VCG
048. ETRON EM73D044VCH
049. ETRON EM73D044SNA
050. ETRON EM73D044SNC
051. ETRON EM73D044SND
052. ETRON EM73D044SNF
053. ETRON EM73E044SNA
054. TOSHIBA TC58CVG0S3H
055. TOSHIBA TC58CVG1S3H
056. TOSHIBA TC58CVG2S0H
057. KIOXIA TC58CVG2S0HRAIJ
058. MICRON MT29F1G01AA
059. MICRON MT29F2G01AA
060. MICRON MT29F4G01AA
061. MICRON MT29F1G01AB
062. MICRON MT29F2G01AB
063. MICRON MT29F4G01AB
064. MICRON MT29F4G01AD
065. HEYANG HYF1GQ4UAACAE
066. HEYANG HYF2GQ4UAACAE
067. HEYANG HYF2GQ4UHCCAE
068. HEYANG HYF1GQ4UDACAE
069. HEYANG HYF2GQ4UDACAE
070. PN PN26G01A-X
071. PN PN26G02A-X
072. PN PN26Q01A-X
073. ATO ATO25D1GA
074. ATO ATO25D2GA
075. ATO ATO25D2GB
076. FM FM25S01
077. FM FM25S01A
078. FM FM25S02A
079. FM FM25G01B
080. FM FM25G02B
081. FM FM25G02C
082. FM FM25G02
083. XTX XT26G02B
084. XTX XT26G01C
085. XTX XT26G02C
086. XTX XT26G01A
087. XTX XT26G02A
088. MIRA PSU1GS20BN
089. BIWIN BWJX08U
090. BIWIN BWET08U
091. FORESEE FS35ND01GD1F1
092. FORESEE FS35ND01GS1F1
093. FORESEE FS35ND02GS2F1
094. FORESEE FS35ND02GD1F1
095. FORESEE FS35ND01GS1Y2
096. FORESEE FS35ND02G-S3Y2
097. FORESEE FS35ND04G-S2Y2
098. DS DS35Q2GA
099. DS DS35M2GA
100. DS DS35Q1GA
101. DS DS35M1GA
102. FISON CS11G0T0A0AA
103. FISON CS11G1T0A0AA
104. FISON CS11G0G0A0AA
105. TYM TYM25D2GA01
106. TYM TYM25D2GA02
107. TYM TYM25D1GA03
108. XINCUN XCSP1AAWH-NT

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
022. F25L016
023. F25L16QA
024. F25L032
025. F25L32QA
026. F25L064
027. F25L64QA
028. GD25Q20C
029. GD25Q40C
030. GD25Q80C
031. GD25LQ80C
032. GD25WD80C
033. GD25WQ80E
034. GD25Q16
035. GD25LQ16C
036. GD25WQ16E
037. GD25Q32
038. GD25LQ32E
039. GD25WQ32E
040. GD25Q64CSIG
041. GD25Q128CSIG
042. GD25F256F
043. GD25Q256CSIG
044. MX25L4005A
045. MX25L8005M
046. MX25L1605D
047. MX25U1635F
048. MX25L3205D
049. MX25U3235F
050. MX25L6405D
051. MX25U6435F
052. MX25L12805D
053. MX25U12835F
054. MX25L25635E
055. MX25U25643G
056. MX25L51245G
057. MX25U51245G
058. YC25Q128
059. FL016AIF
060. FL064AIF
061. S25FL016P
062. S25FL032P
063. S25FL064P
064. S25FL128P
065. S25FL129P
066. S25FL256S
067. S25FL512S
068. S25FL116K
069. S25FL132K
070. S25FL164K
071. EN25F16
072. EN25Q16
073. EN25QH16
074. EN25Q32B
075. EN25F32
076. EN25F64
077. EN25Q64
078. EN25QA64A
079. EN25QH64A
080. EN25Q128
081. EN25Q256
082. EN25QA128A
083. EN25QH128A
084. GM25Q128A
085. W25X05
086. W25X10
087. W25X20
088. W25X40
089. W25X80
090. W25X16
091. W25X32VS
092. W25X64
093. W25Q20CL
094. W25Q20BW
095. W25Q20EW
096. W25Q80
097. W25Q80BL
098. W25Q16JQ
099. W25Q16JM
100. W25Q32BV
101. W25Q32DW
102. W25Q32JWIM
103. W25Q64BV
104. W25Q64DW
105. W25Q64JVIM
106. W25Q64JWIM
107. W25Q128BV
108. W25Q128FW
109. W25Q256FV
110. W25Q256JW
111. W25Q256JWIM
112. W25Q512JV
113. W25Q512JVIM
114. W25Q512NW
115. W25Q512NWIM
116. M25P05
117. M25P10
118. M25P20
119. M25P40
120. M25P80
121. M25P16
122. M25P32
123. M25P64
124. M25P128
125. N25Q016A
126. N25Q032A
127. N25Q032A
128. N25Q064A
129. N25Q064A
130. N25Q128A
131. N25Q128A
132. N25Q256A
133. N25Q512A
134. MT25QL64AB
135. MT25QU64AB
136. MT25QL128AB
137. MT25QU128AB
138. MT25QL256AB
139. MT25QU256AB
140. MT25QL512AB
141. MT25QU512AB
142. XM25QH10B
143. XM25QH20B
144. XM25QU41B
145. XM25QH40B
146. XM25QU80B
147. XM25QH80B
148. XM25QU16B
149. XM25QH16C
150. XM25QW16C
151. XM25QH32B
152. XM25QW32C
153. XM25LU32C
154. XM25QH32A
155. XM25QH64C
156. XM25LU64C
157. XM25QW64C
158. XM25QH64A
159. XM25QH128A
160. XM25QH128C
161. XM25LU128C
162. XM25QW128C
163. XM25QH256C
164. XM25QU256C
165. XM25QW256C
166. XM25QH512C
167. XM25QU512C
168. XM25QW512C
169. MD25D20
170. MD25D40
171. ZB25VQ16
172. ZB25LQ16
173. ZB25VQ32
174. ZB25LQ32
175. ZB25VQ64
176. ZB25LQ64
177. ZB25VQ128
178. ZB25LQ128
179. LE25U20AMB
180. LE25U40CMC
181. BY25D05AS
182. BY25D10AS
183. BY25D20AS
184. BY25D40AS
185. BY25Q40BL
186. BY25Q40BL
187. BY25Q80BS
188. BY25Q16BS
189. BY25Q16BL
190. BY25Q32BS
191. BY25Q32AL
192. BY25Q64AS
193. BY25Q64AL
194. BY25Q128AS
195. BY25Q128EL
196. BY25Q256ES
197. XT25F04D
198. XT25F08B
199. XT25F08D
200. XT25F16B
201. XT25Q16D
202. XT25F32B
203. XT25F64B
204. XT25Q64D
205. XT25F128B
206. XT25F128D
207. PM25LQ016
208. PM25LQ032
209. PM25LQ064
210. PM25LQ128
211. IS25LQ010
212. IS25LQ020
213. IS25WP040D
214. IS25LP080D
215. IS25WP080D
216. IS25LP016D
217. IS25WP016D
218. IS25LP032D
219. IS25WP032D
220. IS25LP064D
221. IS25WP064D
222. IS25LP128F
223. IS25WP128F
224. IS25LP256D
225. IS25WP256D
226. IS25LP256D
227. IS25WP256D
228. FM25W04
229. FM25Q04
230. FM25Q08
231. FM25W16
232. FM25Q16
233. FM25W32
234. FS25Q32
235. FM25W64
236. FS25Q64
237. FM25W128
238. FS25Q128
239. FM25Q04A
240. FM25M04A
241. FM25Q08A
242. FM25M08A
243. FM25Q16A
244. FM25M16A
245. FM25Q32A
246. FM25M32B
247. FM25Q64A
248. FM25M64A
249. FM25Q128A
250. PN25F16
251. PN25F32
252. PN25F64
253. PN25F128
254. P25D05H
255. P25D10H
256. P25D20H
257. P25D40H
258. P25D80H
259. P25Q16H
260. P25Q32H
261. P25Q64H
262. P25Q128H
263. PY25Q128HA
264. SK25P32
265. SK25P64
266. SK25P128
267. ZD25Q16A
268. ZD25Q32A
269. ZD25Q64A
270. ZD25Q128A
271. ZD25Q16B
272. ZD25Q32B
273. ZD25Q64B
274. ZD25Q128B
275. PCT25VF010A
276. PCT25VF020B
277. PCT25VF040B
278. PCT25VF080B
279. PCT25VF016B
280. PCT25VF032B
281. PCT25VF064C
282. PCT26VF016
283. PCT26VF032

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

Originally written by [McMCC](https://github.com/McMCCRU/SNANDer) and released under the terms of the GNU GPL, version 2.0, or later. Modifications by [Droid-MAX](https://github.com/Droid-MAX/) to make it more easier build for Windows. And [ZhiyuanYuanNJ](https://github.com/ZhiyuanYuanNJ/SNANDer) implemented support for the ch347 programmer.

**License**

This is free software: you can redistribute it and/or modify it under the terms of
the latest GNU General Public License as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.
