# [NftMinter](https://eddytheco.github.io/NftMinter/index.html)


This repo produces an application that can mint NFTs on networks of IOTA with stardust protocol.
We are not interested in the speculative side of crypto and we are not associated with the Iota Foundation. We discourage investing in Shimmer and IOTA because that will be investing in the Iota Foundation and its pursuit of centralizing the development and protocol.

To use the application one needs to set the address of the node to connect.
The Proof of Work has to be performed by the node.
This application is meant to be used on the testnet.
If using the mainnet **you are the ONLY responsible for the eventual loss of your funds**.


The application produces a random seed to be able to sign blocks using the 0 index address.
The application is not intended to store funds or NFTs on that seed.
One could also save the produced seed just in case. 
Although you can set a seed  for recovering, **you should not input your personal seeds to the app**(read again the bold phrases).

If requested by the user, the application can store the password-encrypted seed on the local storage of the user.
When using the Browser application the encrypted on the local storage of the browser.


Because we are waiting for IOTA 2.0 protocol changes, the application only supports Basic and NFT Outputs without native tokens.
Native Tokens on the Outputs will be burned.


Watch the [showcase video](https://www.youtube.com/watch?v=UK_493BTI1M)

