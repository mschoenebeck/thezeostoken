var zeos = require('./zeos.js');
const fetch = require("node-fetch");
//const { createClient } = require('@liquidapps/dapp-client');
const { requireBox, getBoxesDir } = require('@liquidapps/box-utils');
const { createClient } = requireBox("client-lib-base/client/dist/src/dapp-client-lib");
global.fetch = fetch;


//var vk_str = '{"alpha_g1":{"x":{"data":[10743384851791489833,16151200417142004433,3486072283009176738,2359704508242334157,4708476084099670530,117978659527193769]},"y":{"data":[13957980117130975030,5822048176370104852,1339558264020996111,1532176014662337896,8340395934102853712,1522755594305001437]},"infinity":{"data":0}},"beta_g1":{"x":{"data":[16336276466035058004,5655388751077479477,1679015989554504750,2623669155576461081,3935913039170219345,154166148320031076]},"y":{"data":[8644335105712336194,12131412578238388818,13121685135007799676,1181734283780229093,7508786078344332975,1626746759878785135]},"infinity":{"data":0}},"beta_g2":{"x":{"c0":{"data":[14683346903100073974,1666718837225419595,5633598055022532106,6513267455984322387,12696530528544167067,582756158093453604]},"c1":{"data":[3420528671943142140,14222245016971183456,2176882552115121043,8801619206015550765,8916460120321652234,173146702777656485]}},"y":{"c0":{"data":[5229810285136009575,10337769438408412402,6268270359993352066,4125159541573391480,779602930788866821,149090053613976319]},"c1":{"data":[2610480942531910736,5475195225999623490,11422087897356630362,14677561971768792069,10838708332151616781,1735190093784600374]}},"infinity":{"data":0}},"gamma_g2":{"x":{"c0":{"data":[2794433170180859567,7064600856466887598,12201975939530329049,9515626992149925907,18157212290257703914,1366911823468187920]},"c1":{"data":[3593013553623095578,8439735514307417609,11842281694234360018,12369263433056225414,8421145686768483154,716645400371106243]}},"y":{"c0":{"data":[8541385706149520710,9071235911008739966,18316841060326806620,15864212296116637958,11395887112706860157,1016488696475916781]},"c1":{"data":[12385799808150148137,12804087564757670396,2328128474431562616,6007774044674591807,12432565084585985708,951634616407578578]}},"infinity":{"data":0}},"delta_g1":{"x":{"data":[8627667892921027099,12165335264705416714,14374326914122215574,11448342927035616156,4075332379949178241,332106812812682765]},"y":{"data":[6110026689438337031,17562828516092098021,9803564303331538501,5776581761860589673,9962648949175825151,216877563980391245]},"infinity":{"data":0}},"delta_g2":{"x":{"c0":{"data":[18059653376801834460,4081407148092412111,6942955549908037155,10121673098419690888,16184483163751819094,1298142748417881665]},"c1":{"data":[9395251418400809222,5418837050998788981,524227774794659679,16868833676259445004,5263494482361274251,585646204315210087]}},"y":{"c0":{"data":[16739784735331559919,1648743447891671405,8846660640161867074,17732140527320760345,9938171135438388642,13490147628417654]},"c1":{"data":[10594112773914910438,15402869028253737547,15602805270476857782,1001640825967860647,11908682065912839655,458958534053967746]}},"infinity":{"data":0}},"ic":[{"x":{"data":[15346966027547093818,6411204378235976676,5234623727307616737,17680489148913946050,8936698622347372480,1003524326246725090]},"y":{"data":[454443256999861957,53440351173102560,9870679934901729919,18193802346729527754,11274054807968347382,658109242880508951]},"infinity":{"data":0}},{"x":{"data":[2660250119366267882,7344684222782227679,7359655539248956854,191157781886424297,14886754476477574807,736234593478700187]},"y":{"data":[2010863625819379920,4886564817472820955,3280047083358996279,4666658994511466890,17118581219092364687,1061308953101415749]},"infinity":{"data":0}},{"x":{"data":[10758570710829461064,11689881356114913902,2913550897319063237,7782348307822869276,216281277844080332,1125599416512488514]},"y":{"data":[1159011061255085614,12957425220883659835,7895363425667529718,11961224213543660359,5263886183803976470,1623618169521695125]},"infinity":{"data":0}}]}';

//var proof_str = '{"a":{"x":{"data":[11281214743188095108,7324960852881245773,16323074521503261530,7935442180524566485,3447205503407154254,142696605983680825]},"y":{"data":[12959333984886701833,7557107698944420430,10229267066654835700,4267746412519327896,683852416278172248,685037468885050226]},"infinity":{"data":0}},"b":{"x":{"c0":{"data":[4489172262238247121,65327003249188103,9875490749400609176,9708019759866291059,13811407545776824185,605904864226976272]},"c1":{"data":[10857120443589108310,8117251748703107106,4071018462117451686,9539327385730157308,5187384769016239525,1229022557506643579]}},"y":{"c0":{"data":[12371344812501916242,13880608924742898686,12684360797548923548,7908539914859763307,747934384853808855,291739538199426247]},"c1":{"data":[8700430337445437660,2568693626588558167,12346231620464952391,10857595540458505831,2666845546159284308,354222503363084332]}},"infinity":{"data":0}},"c":{"x":{"data":[685179753558974742,18240946522920941153,8481567336493327996,5233494953992381856,5791281528757539263,1501267797956638369]},"y":{"data":[13024671334677689105,17824851971584033097,8770117755147595354,11381201407111114616,239678930193425203,953532897345263433]},"infinity":{"data":0}}}';

//var inputs_str = '[{"data":[11280786627794629505,15465988939752547151,1510186046958900449,7392355891667949747]},{"data":[0,0,0,0]}]';

//var vk_b64 = "eyJhbHBoYV9nMSI6eyJ4Ijp7ImRhdGEiOlsxMDc0MzM4NDg1MTc5MTQ4OTgzMywxNjE1MTIwMDQxNzE0MjAwNDQzMywzNDg2MDcyMjgzMDA5MTc2NzM4LDIzNTk3MDQ1MDgyNDIzMzQxNTcsNDcwODQ3NjA4NDA5OTY3MDUzMCwxMTc5Nzg2NTk1MjcxOTM3NjldfSwieSI6eyJkYXRhIjpbMTM5NTc5ODAxMTcxMzA5NzUwMzAsNTgyMjA0ODE3NjM3MDEwNDg1MiwxMzM5NTU4MjY0MDIwOTk2MTExLDE1MzIxNzYwMTQ2NjIzMzc4OTYsODM0MDM5NTkzNDEwMjg1MzcxMiwxNTIyNzU1NTk0MzA1MDAxNDM3XX0sImluZmluaXR5Ijp7ImRhdGEiOjB9fSwiYmV0YV9nMSI6eyJ4Ijp7ImRhdGEiOlsxNjMzNjI3NjQ2NjAzNTA1ODAwNCw1NjU1Mzg4NzUxMDc3NDc5NDc3LDE2NzkwMTU5ODk1NTQ1MDQ3NTAsMjYyMzY2OTE1NTU3NjQ2MTA4MSwzOTM1OTEzMDM5MTcwMjE5MzQ1LDE1NDE2NjE0ODMyMDAzMTA3Nl19LCJ5Ijp7ImRhdGEiOls4NjQ0MzM1MTA1NzEyMzM2MTk0LDEyMTMxNDEyNTc4MjM4Mzg4ODE4LDEzMTIxNjg1MTM1MDA3Nzk5Njc2LDExODE3MzQyODM3ODAyMjkwOTMsNzUwODc4NjA3ODM0NDMzMjk3NSwxNjI2NzQ2NzU5ODc4Nzg1MTM1XX0sImluZmluaXR5Ijp7ImRhdGEiOjB9fSwiYmV0YV9nMiI6eyJ4Ijp7ImMwIjp7ImRhdGEiOlsxNDY4MzM0NjkwMzEwMDA3Mzk3NCwxNjY2NzE4ODM3MjI1NDE5NTk1LDU2MzM1OTgwNTUwMjI1MzIxMDYsNjUxMzI2NzQ1NTk4NDMyMjM4NywxMjY5NjUzMDUyODU0NDE2NzA2Nyw1ODI3NTYxNTgwOTM0NTM2MDRdfSwiYzEiOnsiZGF0YSI6WzM0MjA1Mjg2NzE5NDMxNDIxNDAsMTQyMjIyNDUwMTY5NzExODM0NTYsMjE3Njg4MjU1MjExNTEyMTA0Myw4ODAxNjE5MjA2MDE1NTUwNzY1LDg5MTY0NjAxMjAzMjE2NTIyMzQsMTczMTQ2NzAyNzc3NjU2NDg1XX19LCJ5Ijp7ImMwIjp7ImRhdGEiOls1MjI5ODEwMjg1MTM2MDA5NTc1LDEwMzM3NzY5NDM4NDA4NDEyNDAyLDYyNjgyNzAzNTk5OTMzNTIwNjYsNDEyNTE1OTU0MTU3MzM5MTQ4MCw3Nzk2MDI5MzA3ODg4NjY4MjEsMTQ5MDkwMDUzNjEzOTc2MzE5XX0sImMxIjp7ImRhdGEiOlsyNjEwNDgwOTQyNTMxOTEwNzM2LDU0NzUxOTUyMjU5OTk2MjM0OTAsMTE0MjIwODc4OTczNTY2MzAzNjIsMTQ2Nzc1NjE5NzE3Njg3OTIwNjksMTA4Mzg3MDgzMzIxNTE2MTY3ODEsMTczNTE5MDA5Mzc4NDYwMDM3NF19fSwiaW5maW5pdHkiOnsiZGF0YSI6MH19LCJnYW1tYV9nMiI6eyJ4Ijp7ImMwIjp7ImRhdGEiOlsyNzk0NDMzMTcwMTgwODU5NTY3LDcwNjQ2MDA4NTY0NjY4ODc1OTgsMTIyMDE5NzU5Mzk1MzAzMjkwNDksOTUxNTYyNjk5MjE0OTkyNTkwNywxODE1NzIxMjI5MDI1NzcwMzkxNCwxMzY2OTExODIzNDY4MTg3OTIwXX0sImMxIjp7ImRhdGEiOlszNTkzMDEzNTUzNjIzMDk1NTc4LDg0Mzk3MzU1MTQzMDc0MTc2MDksMTE4NDIyODE2OTQyMzQzNjAwMTgsMTIzNjkyNjM0MzMwNTYyMjU0MTQsODQyMTE0NTY4Njc2ODQ4MzE1NCw3MTY2NDU0MDAzNzExMDYyNDNdfX0sInkiOnsiYzAiOnsiZGF0YSI6Wzg1NDEzODU3MDYxNDk1MjA3MTAsOTA3MTIzNTkxMTAwODczOTk2NiwxODMxNjg0MTA2MDMyNjgwNjYyMCwxNTg2NDIxMjI5NjExNjYzNzk1OCwxMTM5NTg4NzExMjcwNjg2MDE1NywxMDE2NDg4Njk2NDc1OTE2NzgxXX0sImMxIjp7ImRhdGEiOlsxMjM4NTc5OTgwODE1MDE0ODEzNywxMjgwNDA4NzU2NDc1NzY3MDM5NiwyMzI4MTI4NDc0NDMxNTYyNjE2LDYwMDc3NzQwNDQ2NzQ1OTE4MDcsMTI0MzI1NjUwODQ1ODU5ODU3MDgsOTUxNjM0NjE2NDA3NTc4NTc4XX19LCJpbmZpbml0eSI6eyJkYXRhIjowfX0sImRlbHRhX2cxIjp7IngiOnsiZGF0YSI6Wzg2Mjc2Njc4OTI5MjEwMjcwOTksMTIxNjUzMzUyNjQ3MDU0MTY3MTQsMTQzNzQzMjY5MTQxMjIyMTU1NzQsMTE0NDgzNDI5MjcwMzU2MTYxNTYsNDA3NTMzMjM3OTk0OTE3ODI0MSwzMzIxMDY4MTI4MTI2ODI3NjVdfSwieSI6eyJkYXRhIjpbNjExMDAyNjY4OTQzODMzNzAzMSwxNzU2MjgyODUxNjA5MjA5ODAyMSw5ODAzNTY0MzAzMzMxNTM4NTAxLDU3NzY1ODE3NjE4NjA1ODk2NzMsOTk2MjY0ODk0OTE3NTgyNTE1MSwyMTY4Nzc1NjM5ODAzOTEyNDVdfSwiaW5maW5pdHkiOnsiZGF0YSI6MH19LCJkZWx0YV9nMiI6eyJ4Ijp7ImMwIjp7ImRhdGEiOlsxODA1OTY1MzM3NjgwMTgzNDQ2MCw0MDgxNDA3MTQ4MDkyNDEyMTExLDY5NDI5NTU1NDk5MDgwMzcxNTUsMTAxMjE2NzMwOTg0MTk2OTA4ODgsMTYxODQ0ODMxNjM3NTE4MTkwOTQsMTI5ODE0Mjc0ODQxNzg4MTY2NV19LCJjMSI6eyJkYXRhIjpbOTM5NTI1MTQxODQwMDgwOTIyMiw1NDE4ODM3MDUwOTk4Nzg4OTgxLDUyNDIyNzc3NDc5NDY1OTY3OSwxNjg2ODgzMzY3NjI1OTQ0NTAwNCw1MjYzNDk0NDgyMzYxMjc0MjUxLDU4NTY0NjIwNDMxNTIxMDA4N119fSwieSI6eyJjMCI6eyJkYXRhIjpbMTY3Mzk3ODQ3MzUzMzE1NTk5MTksMTY0ODc0MzQ0Nzg5MTY3MTQwNSw4ODQ2NjYwNjQwMTYxODY3MDc0LDE3NzMyMTQwNTI3MzIwNzYwMzQ1LDk5MzgxNzExMzU0MzgzODg2NDIsMTM0OTAxNDc2Mjg0MTc2NTRdfSwiYzEiOnsiZGF0YSI6WzEwNTk0MTEyNzczOTE0OTEwNDM4LDE1NDAyODY5MDI4MjUzNzM3NTQ3LDE1NjAyODA1MjcwNDc2ODU3NzgyLDEwMDE2NDA4MjU5Njc4NjA2NDcsMTE5MDg2ODIwNjU5MTI4Mzk2NTUsNDU4OTU4NTM0MDUzOTY3NzQ2XX19LCJpbmZpbml0eSI6eyJkYXRhIjowfX0sImljIjpbeyJ4Ijp7ImRhdGEiOlsxNTM0Njk2NjAyNzU0NzA5MzgxOCw2NDExMjA0Mzc4MjM1OTc2Njc2LDUyMzQ2MjM3MjczMDc2MTY3MzcsMTc2ODA0ODkxNDg5MTM5NDYwNTAsODkzNjY5ODYyMjM0NzM3MjQ4MCwxMDAzNTI0MzI2MjQ2NzI1MDkwXX0sInkiOnsiZGF0YSI6WzQ1NDQ0MzI1Njk5OTg2MTk1Nyw1MzQ0MDM1MTE3MzEwMjU2MCw5ODcwNjc5OTM0OTAxNzI5OTE5LDE4MTkzODAyMzQ2NzI5NTI3NzU0LDExMjc0MDU0ODA3OTY4MzQ3MzgyLDY1ODEwOTI0Mjg4MDUwODk1MV19LCJpbmZpbml0eSI6eyJkYXRhIjowfX0seyJ4Ijp7ImRhdGEiOlsyNjYwMjUwMTE5MzY2MjY3ODgyLDczNDQ2ODQyMjI3ODIyMjc2NzksNzM1OTY1NTUzOTI0ODk1Njg1NCwxOTExNTc3ODE4ODY0MjQyOTcsMTQ4ODY3NTQ0NzY0Nzc1NzQ4MDcsNzM2MjM0NTkzNDc4NzAwMTg3XX0sInkiOnsiZGF0YSI6WzIwMTA4NjM2MjU4MTkzNzk5MjAsNDg4NjU2NDgxNzQ3MjgyMDk1NSwzMjgwMDQ3MDgzMzU4OTk2Mjc5LDQ2NjY2NTg5OTQ1MTE0NjY4OTAsMTcxMTg1ODEyMTkwOTIzNjQ2ODcsMTA2MTMwODk1MzEwMTQxNTc0OV19LCJpbmZpbml0eSI6eyJkYXRhIjowfX0seyJ4Ijp7ImRhdGEiOlsxMDc1ODU3MDcxMDgyOTQ2MTA2NCwxMTY4OTg4MTM1NjExNDkxMzkwMiwyOTEzNTUwODk3MzE5MDYzMjM3LDc3ODIzNDgzMDc4MjI4NjkyNzYsMjE2MjgxMjc3ODQ0MDgwMzMyLDExMjU1OTk0MTY1MTI0ODg1MTRdfSwieSI6eyJkYXRhIjpbMTE1OTAxMTA2MTI1NTA4NTYxNCwxMjk1NzQyNTIyMDg4MzY1OTgzNSw3ODk1MzYzNDI1NjY3NTI5NzE4LDExOTYxMjI0MjEzNTQzNjYwMzU5LDUyNjM4ODYxODM4MDM5NzY0NzAsMTYyMzYxODE2OTUyMTY5NTEyNV19LCJpbmZpbml0eSI6eyJkYXRhIjowfX1dfQ==";

//var proof_b64 = "eyJhIjp7IngiOnsiZGF0YSI6WzExMjgxMjE0NzQzMTg4MDk1MTA4LDczMjQ5NjA4NTI4ODEyNDU3NzMsMTYzMjMwNzQ1MjE1MDMyNjE1MzAsNzkzNTQ0MjE4MDUyNDU2NjQ4NSwzNDQ3MjA1NTAzNDA3MTU0MjU0LDE0MjY5NjYwNTk4MzY4MDgyNV19LCJ5Ijp7ImRhdGEiOlsxMjk1OTMzMzk4NDg4NjcwMTgzMyw3NTU3MTA3Njk4OTQ0NDIwNDMwLDEwMjI5MjY3MDY2NjU0ODM1NzAwLDQyNjc3NDY0MTI1MTkzMjc4OTYsNjgzODUyNDE2Mjc4MTcyMjQ4LDY4NTAzNzQ2ODg4NTA1MDIyNl19LCJpbmZpbml0eSI6eyJkYXRhIjowfX0sImIiOnsieCI6eyJjMCI6eyJkYXRhIjpbNDQ4OTE3MjI2MjIzODI0NzEyMSw2NTMyNzAwMzI0OTE4ODEwMyw5ODc1NDkwNzQ5NDAwNjA5MTc2LDk3MDgwMTk3NTk4NjYyOTEwNTksMTM4MTE0MDc1NDU3NzY4MjQxODUsNjA1OTA0ODY0MjI2OTc2MjcyXX0sImMxIjp7ImRhdGEiOlsxMDg1NzEyMDQ0MzU4OTEwODMxMCw4MTE3MjUxNzQ4NzAzMTA3MTA2LDQwNzEwMTg0NjIxMTc0NTE2ODYsOTUzOTMyNzM4NTczMDE1NzMwOCw1MTg3Mzg0NzY5MDE2MjM5NTI1LDEyMjkwMjI1NTc1MDY2NDM1NzldfX0sInkiOnsiYzAiOnsiZGF0YSI6WzEyMzcxMzQ0ODEyNTAxOTE2MjQyLDEzODgwNjA4OTI0NzQyODk4Njg2LDEyNjg0MzYwNzk3NTQ4OTIzNTQ4LDc5MDg1Mzk5MTQ4NTk3NjMzMDcsNzQ3OTM0Mzg0ODUzODA4ODU1LDI5MTczOTUzODE5OTQyNjI0N119LCJjMSI6eyJkYXRhIjpbODcwMDQzMDMzNzQ0NTQzNzY2MCwyNTY4NjkzNjI2NTg4NTU4MTY3LDEyMzQ2MjMxNjIwNDY0OTUyMzkxLDEwODU3NTk1NTQwNDU4NTA1ODMxLDI2NjY4NDU1NDYxNTkyODQzMDgsMzU0MjIyNTAzMzYzMDg0MzMyXX19LCJpbmZpbml0eSI6eyJkYXRhIjowfX0sImMiOnsieCI6eyJkYXRhIjpbNjg1MTc5NzUzNTU4OTc0NzQyLDE4MjQwOTQ2NTIyOTIwOTQxMTUzLDg0ODE1NjczMzY0OTMzMjc5OTYsNTIzMzQ5NDk1Mzk5MjM4MTg1Niw1NzkxMjgxNTI4NzU3NTM5MjYzLDE1MDEyNjc3OTc5NTY2MzgzNjldfSwieSI6eyJkYXRhIjpbMTMwMjQ2NzEzMzQ2Nzc2ODkxMDUsMTc4MjQ4NTE5NzE1ODQwMzMwOTcsODc3MDExNzc1NTE0NzU5NTM1NCwxMTM4MTIwMTQwNzExMTExNDYxNiwyMzk2Nzg5MzAxOTM0MjUyMDMsOTUzNTMyODk3MzQ1MjYzNDMzXX0sImluZmluaXR5Ijp7ImRhdGEiOjB9fX0=";

//var inputs_b64 = "W3siZGF0YSI6WzExMjgwNzg2NjI3Nzk0NjI5NTA1LDE1NDY1OTg4OTM5NzUyNTQ3MTUxLDE1MTAxODYwNDY5NTg5MDA0NDksNzM5MjM1NTg5MTY2Nzk0OTc0N119LHsiZGF0YSI6WzAsMCwwLDBdfV0=";

module.exports = async ({ proto, address }) => {
  // zeos_verify_proof://  
  
  // split address to extract parameters
  const payloadParts = address.split('/');
  let idx = 0;
  var vk_code = payloadParts[idx++];
  var vk_id = payloadParts[idx++];
  var proof_str = payloadParts[idx++];
  var inputs_str = payloadParts[idx++];
  
  // fetch vk from vram
  let dappClient = await createClient({ httpEndpoint: "http://localhost:13015", fetch });
  let vramClient = await dappClient.service('ipfs', "thezeostoken");
  var response = await vramClient.get_vram_row("thezeostoken", vk_code, "verifierkey", vk_id);
  var vk_str = response.row.vk;
  
  var res;
  await zeos().then((instance) => {
    var vk_ptr = instance.allocate(instance.intArrayFromString(vk_str), instance.ALLOC_NORMAL);
    var proof_ptr = instance.allocate(instance.intArrayFromString(proof_str), instance.ALLOC_NORMAL);
    var inputs_ptr = instance.allocate(instance.intArrayFromString(inputs_str), instance.ALLOC_NORMAL);
  
    res = instance._verify_proof(vk_ptr, proof_ptr, inputs_ptr);
  }); 

  if(res)
  {
    res = "1";
  }
  else
  {
    res = "0";
  }
  
  return new Buffer(res, "utf-8");
};
