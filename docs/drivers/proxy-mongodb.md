# Http:MongoDB
A [http api proxy](https://github.com/Arkensor/EnfusionDatabaseFramework.Drivers.WebProxy.MongoDB) based driver for the [MongoDB](https://mongodb.com) document database. Excellent storage for the highly unstructured data that results from the entity component system in Arma Reforger.  

> **Note**
> Consider manually adding query indicies to your MongoDB server for collections that are often queried with complex conditions. This can signifantly reduce query time on large datasets.

### Implementation: [`EDF_MongoDbDriver`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/WebProxy/EDF_MongoDbDriver.c;7)

### Aliases: `MongoDb`, `Mongo`

### ConnectionInfo: [`EDF_MongoDbConnectionInfo`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/WebProxy/EDF_MongoDbDriver.c;2)
| Option           | Values      | Description                                                              |
|------------------|-------------|--------------------------------------------------------------------------|
| ProxyHost        | Hostname/IP | Web proxy hostname.                                                      |
| ProxyPort        | Portnumber  | Web proxy port.                                                          |
| SecureConnection | True/False  | Use TLS/SSL to connect to the web proxy.                                 |
| Parameters       | key=value   | Additional parameters added to the url with ...&key=value e.g. api keys. |
