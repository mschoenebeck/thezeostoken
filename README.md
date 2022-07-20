# ZEOS Token Contract

This is the smart contract for the [ZEOS](https://zeos.one) protocol for private and untraceable transactions on the EOS Blockchain. This contract is deployed to the [Kylin Testnet](https://kylin.bloks.io/account/thezeostoken) and the [EOS Mainnet](https://bloks.io/account/thezeostoken).

See also:
- [The ZEOS Book](https://mschoenebeck.github.io/zeos-orchard/) (including a full protocol specification)
- [ZEOS Sapling](https://github.com/mschoenebeck/zeos-sapling)
- [ZEOS Orchard](https://github.com/mschoenebeck/zeos-orchard)

## Description
This contract issues the ZEOS tokens as well as all zAsset token types that will be added to the protocol in the future. This contract features the 'verifyproof' action which verfies zk-SNARKs. It currently supports the [Groth16](https://electriccoin.co/blog/bellman-zksnarks-in-rust/) proving system as well as [Halo2](https://zcash.github.io/halo2/index.html). The 'verifyproof' action depends on the [vCPU](https://liquidapps.io/vcpu) service of LiquidApps. Check out the [ZEOS Verifier](https://github.com/mschoenebeck/zeos-verifier) to learn more about the vCPU module this contract depends on.

This contract is built on [EOSIO](https://eos.io/) and [Liquidapps' DAPP Network](https://liquidapps.io/) services.

## Getting Started

Clone this repository:

```
git clone https://github.com/mschoenebeck/thezeostoken.git
cd thezeostoken
```

Build the project with Zeus:

```
zeus compile
```

Run the unit tests with Zeus:

```
zeus test
```

### Dependencies

- [EOSIO Contract Development Toolkit (CDT)](https://github.com/EOSIO/eosio.cdt/releases)
- [LiquidApps Zeus SDK](https://liquidapps.io/zeus)

## Help
If you need help join us on [Telegram](https://t.me/ZeosOnEos).

## Authors

Matthias Schönebeck

## License

It's open source. Do with it whatever you want.

## Acknowledgments

Big thanks to the Electric Coin Company for developing, documenting and maintaining this awesome open source codebase for zk-SNARKs!

* [Zcash Protocol Specification](https://zips.z.cash/protocol/protocol.pdf)