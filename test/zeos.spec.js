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
const authContractCode = "authenticator";

var ctrtStorage = artifacts.require(`./${contractCode}/`);
var ctrtAuth = artifacts.require(`./${authContractCode}/`);

describe(`ZEOS Test`, async () => {
  var testcontract;
  const code = "thezeostoken";
  let dappClient;
  let storageClient
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
        storageClient = await dappClient.service("storage", code);
        var deployedStorage = await deployer.deploy(ctrtStorage, code);
        var deployedAuth = await deployer.deploy(ctrtAuth, "authentikeos");

        await genAllocateDAPPTokens(deployedStorage, "storage");
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
  it('ZEOS create', done => {
    (async () => {
      try {
        var res = await testcontract.create({
          issuer: "thezeostoken",
          maximum_supply: "1000000000 ZEOS",
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
  it('ZEOS issue', done => {
    (async () => {
      try {
        var res = await testcontract.issue({
          to: "thezeostoken",
          quantity: "1000000000 ZEOS",
          memo: "first issue",
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
