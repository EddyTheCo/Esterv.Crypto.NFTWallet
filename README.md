# [NftMinter](https://eddytheco.github.io/NftMinter/index.html)


This repo produces an application that can mint NFTs on networks of IOTA with stardust protocol.

To use the application one needs to set the address of the node to connect.
The Proof of Work has to be performed by the node (by setting the JWT for protected routes, by enabling PoW in the node...).
This application is meant to be used on the testnet.
If using the mainnet **you are the ONLY responsible for the eventual loss of your funds**.


The application produces a random seed to be able to sign blocks using the 0 index address.
The application is not intended to store funds or NFTs on that seed.
One could also save the produced seed just in case. 
Although you can set a seed  for recovering, **you should not input your personal seeds to the app**(read again the bold phrases).

Watch the [showcase video](https://www.youtube.com/watch?v=UK_493BTI1M)

### CORS header 'Access-Control-Allow-Origin' missing

When using the browser application and your node, the API request could be denied with the return 'Reason: CORS header 'Access-Control-Allow-Origin' missing'.
In that case, one needs to set the Access-Control-Allow-Origin header's value as explained [here](https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS/Errors/CORSMissingAllowOrigin).

If you use the docker setup of Hornet just add 

```
- "traefik.http.middlewares.cors.headers.customResponseHeaders.Access-Control-Allow-Origin=https://eddytheco.github.io"
- "traefik.http.routers.hornet.middlewares=cors"
```
to docker-compose.yml in the traefik section. Such that browser API requests from https://eddytheco.github.io are accepted  by your node.
