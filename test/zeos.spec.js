require("mocha");
const { requireBox, getBoxesDir } = require('@liquidapps/box-utils');
const fs = require("fs");
const { assert } = require("chai"); // Using Assert style
const fetch = require("node-fetch");
const ecc = require("eosjs-ecc");
const { JsonRpc } = require("eosjs");
const {
  getTestContract,
  getCreateKeys,
} = requireBox('seed-eos/tools/eos/utils');
const { createClient } = requireBox("client-lib-base/client/dist/src/dapp-client-lib");
const artifacts = requireBox('seed-eos/tools/eos/artifacts');
const deployer = requireBox('seed-eos/tools/eos/deployer');
const { genAllocateDAPPTokens } = requireBox('dapp-services/tools/eos/dapp-services');
const { getIpfsFileAsBuffer } = requireBox("storage-dapp-service/services/storage-dapp-service-node/common.js")

//dappclient requirement
global.fetch = fetch;
var endpoint = "http://localhost:13015";

const rpc = new JsonRpc(endpoint, { fetch });

const contractCode = "thezeostoken";

var ctrtStorage = artifacts.require(`./${contractCode}/`);

describe(`ZEOS Test`, async () => {
  var testcontract;
  const code = "thezeostoken";
  let dappClient;
  const boxDir = getBoxesDir();
  const permission = "active";
  const keys = await getCreateKeys(code);
  const key = keys.active.privateKey;

  before(done => {
    (async () => {
      try {
        dappClient = await createClient({
          httpEndpoint: endpoint,
          fetch,
        });
        vramClient = await dappClient.service('ipfs', code);
        var deployedStorage = await deployer.deploy(ctrtStorage, code);

        await genAllocateDAPPTokens(deployedStorage, "ipfs");
        await genAllocateDAPPTokens(deployedStorage, "oracle", "pprovider1", "default");
        await genAllocateDAPPTokens(deployedStorage, "oracle", "pprovider2", "foobar");
        testcontract = await getTestContract(code);

        let info = await rpc.get_info();
        let chainId = info.chain_id;

        done();
      } catch (e) {
        done(e);
      }
    })();
  });
  it('ZEOS set verifier key', done => {
    (async () => {
      try {
        var res = await testcontract.setvk({
          code: "thezeostoken",
          id: "zeosgrothkey",
          vk: "14590b8a5810e748809975f79bb818a739b8f26770879eb834e9720117ebbec7a5b8c9b17886687428fc17982e00808e0141c3ddd4044c37fcd6f50fdc6f7c0e026e58cd7626996c59421377e9bdee23ab197b41469b4bd02bd94993c91c55180063ec11f58e351eb06c1a666153c9afd3a7a9828d0cbb4649f36f74b58d680a286a5ae143a06781c4e42eef537240c101ec066c0da8327d8f7a615acd5219c30a2d74d31aa21208d97cb36b7667487ba06449cedbbe8aa26bd82bca7213654108aee00c52b826dfa80f025414255ae597b8921ae9bb57438137e90032e299f700b3da8ee6899b4312df140b18b7452f0f6aa8ead1e950afe80aae9e30c7df9eb2addf41f0398f03168fda5c8a7aee305bbc3570507f282976fcc3afcfde8fc1095bd5b5486d77d8b113cb4c011732857a855f7d085dbd7427f94230000510e4d9c5dddce9009de4d89d68976412bb80049f08b4c6804306e0faf378a30bbcff3ded2fb5f71ca036a9641489ce2004f00f6314cca6efcc3110d20b2a7fec0cd102a87460cb3705ef2f5d8f3ba6303c544b580426fd3175186ebea182ed0e69f8e9e575fa15a07bf1a5cad6065043baa00023ecd090a1a4e1d02f055ffc49ef825dd357e92e0ff363792d8e7193a7b474d5d4bf456110c8f92f18525ea0c1568d092450b7ca77930e2a5ebf592d87b7b64c3ef5aa9e9f61e4597a06faf31ec4622576aa84dcbf2be9375e7a366a0741170e8b800579df0719c433b7f189d7408390841e895c6b3d341297b933f553d806559559a04f4c8972b9a1471608879e580f9be2b7148d152518a534e9210051d885ee936605da0fba97e9666bd9f291119d39600a2fb1484fb9f1907ec5bf211e0a74c9f4d023ac99cf938f11acabec86428e23b1451d02c3249aad4041f3ef66c9b8a2ec531bc6dbd49061d6cb649988119bc4fb25a669eb12b092a002136ace1184fc7237107f32276e20bcda8a7c8bb5063f5294e072e4d9c468c3fb1d387419d6d5ffd3338287ae6ca684d055204ee2f81efa6d54eb218299f51575c1d12ed619133cf2abf8ecd62f685dd5da8b6606e53491e164bdd71f1535c35329a19a60ce892955f8c6d1a4de03d8da6eae1fa4eeae5c8c0c9e041cd76f2ae8495b080ce08861b6df3f23b2331fb96337d270f5b8bb99b0009e4237494133cdfafa83747d31a18c6ab8d9060ea33e7d14ff5e000000030c38b9c8670f540c17cf9b033a4ae659c206c8662c6a2739a7c02b9f814b2e14a948c9606991b783de2945fd932ed2591548f8daa4bc35780949c79d346e001a3883cdaeaf89720a95f2ef046b2c6c02d6d9867bea27c186e250bd53e72ffc081319f58eff6d4cbb047be675df5c4e12234dea8a24983011a2edc6e4eb8fae5355a7d51bb425735b6237e6a619db965d0f76f13130d79d57c1ac71c5acea06b1ed536e315290b57a4e4355013c55ab76233190e085c599125a4ee73f1edc70f514623fbaac7447545f5bb35b6600b778d596e0f133f268e8a776941f6e645f8d7e650001d2a42b8808cacce15ba23acd0b903d5664607981ffe4df61c84b526818884f791f443aa126278737c6a8db552b2eddb134b7838f1e08e16452b34114",
        }, {
          authorization: `${code}@active`,
          broadcast: true,
          sign: true
        });
        done();
      }
      catch (e) {
        done(e);
      }
    })();
  });
  it("check vk", done => {
    (async () => {
      try {
        let response = await vramClient.get_vram_row("thezeostoken", "thezeostoken", "vk", "zeosgrothkey");
        assert.equal(response.row.vk, "14590b8a5810e748809975f79bb818a739b8f26770879eb834e9720117ebbec7a5b8c9b17886687428fc17982e00808e0141c3ddd4044c37fcd6f50fdc6f7c0e026e58cd7626996c59421377e9bdee23ab197b41469b4bd02bd94993c91c55180063ec11f58e351eb06c1a666153c9afd3a7a9828d0cbb4649f36f74b58d680a286a5ae143a06781c4e42eef537240c101ec066c0da8327d8f7a615acd5219c30a2d74d31aa21208d97cb36b7667487ba06449cedbbe8aa26bd82bca7213654108aee00c52b826dfa80f025414255ae597b8921ae9bb57438137e90032e299f700b3da8ee6899b4312df140b18b7452f0f6aa8ead1e950afe80aae9e30c7df9eb2addf41f0398f03168fda5c8a7aee305bbc3570507f282976fcc3afcfde8fc1095bd5b5486d77d8b113cb4c011732857a855f7d085dbd7427f94230000510e4d9c5dddce9009de4d89d68976412bb80049f08b4c6804306e0faf378a30bbcff3ded2fb5f71ca036a9641489ce2004f00f6314cca6efcc3110d20b2a7fec0cd102a87460cb3705ef2f5d8f3ba6303c544b580426fd3175186ebea182ed0e69f8e9e575fa15a07bf1a5cad6065043baa00023ecd090a1a4e1d02f055ffc49ef825dd357e92e0ff363792d8e7193a7b474d5d4bf456110c8f92f18525ea0c1568d092450b7ca77930e2a5ebf592d87b7b64c3ef5aa9e9f61e4597a06faf31ec4622576aa84dcbf2be9375e7a366a0741170e8b800579df0719c433b7f189d7408390841e895c6b3d341297b933f553d806559559a04f4c8972b9a1471608879e580f9be2b7148d152518a534e9210051d885ee936605da0fba97e9666bd9f291119d39600a2fb1484fb9f1907ec5bf211e0a74c9f4d023ac99cf938f11acabec86428e23b1451d02c3249aad4041f3ef66c9b8a2ec531bc6dbd49061d6cb649988119bc4fb25a669eb12b092a002136ace1184fc7237107f32276e20bcda8a7c8bb5063f5294e072e4d9c468c3fb1d387419d6d5ffd3338287ae6ca684d055204ee2f81efa6d54eb218299f51575c1d12ed619133cf2abf8ecd62f685dd5da8b6606e53491e164bdd71f1535c35329a19a60ce892955f8c6d1a4de03d8da6eae1fa4eeae5c8c0c9e041cd76f2ae8495b080ce08861b6df3f23b2331fb96337d270f5b8bb99b0009e4237494133cdfafa83747d31a18c6ab8d9060ea33e7d14ff5e000000030c38b9c8670f540c17cf9b033a4ae659c206c8662c6a2739a7c02b9f814b2e14a948c9606991b783de2945fd932ed2591548f8daa4bc35780949c79d346e001a3883cdaeaf89720a95f2ef046b2c6c02d6d9867bea27c186e250bd53e72ffc081319f58eff6d4cbb047be675df5c4e12234dea8a24983011a2edc6e4eb8fae5355a7d51bb425735b6237e6a619db965d0f76f13130d79d57c1ac71c5acea06b1ed536e315290b57a4e4355013c55ab76233190e085c599125a4ee73f1edc70f514623fbaac7447545f5bb35b6600b778d596e0f133f268e8a776941f6e645f8d7e650001d2a42b8808cacce15ba23acd0b903d5664607981ffe4df61c84b526818884f791f443aa126278737c6a8db552b2eddb134b7838f1e08e16452b34114".toUpperCase());
        done();
      } catch (e) {
        console.log(e);
        done(e);
      }
    })();
  });
  it('ZEOS verify proof', done => {
    (async () => {
      try {
        var res = await testcontract.verifyproof({
          type: "groth16",
          code: "thezeostoken",
          id: "zeosgrothkey",
          proof: "981855436af9b3bbdf87da586d9136cda3dbdf04000ec5d0df1ad8a2e3649033411a164efc183406f8d0f1ec1584e7e5849b3dbfa163e8744fe7d12f649c74b5ef444531a08ca85a87c567ddc3e0de0757b8fdf8222ef6890b1291549ca4f2751381726462c0fcd8894b509d3cf9ce0fe9f8d61f28399ddb6de2f0e63396766dec99db7f0da5e3c939dc5d28aa4aa9a998c4369acafee766c6884146ddd1012df0a6097f94f96ef84dabbb1e9bd37730fe419901ee5abe98e517def6fbd708ab",
          inputs: "0203a2313304977f312fdff49de507586a8da7b7dc0f08770c52da0ecf00491b050000000000000000000000000000000000000000000000000000000000000000",
        }, {
          authorization: `${code}@active`,
          broadcast: true,
          sign: true
        });
        done();
      }
      catch (e) {
        done(e);
      }
    })();
  });


});
