# JsonFile
Local `.json` files for workbench development and *small* data volumes. **Do not use** for high-frequency complex queries on larger collections. Can however scale infinitely with the same speed on find by ID-only queries. Consult query performance metrics and if causing frame time problems consider switching to a "real" database via `Http` proxy.

### Implementation: [`EDF_JsonFileDbDriver`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/LocalFile/EDF_JsonFileDbDriver.c;16)

### Aliases: `Json`

### ConnectionInfo: [`EDF_JsonFileDbConnectionInfo`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/LocalFile/EDF_JsonFileDbDriver.c;2)
| Option   | Values     | Description                                                                       |
|----------|------------|-----------------------------------------------------------------------------------|
| Cache    | True/False | Cache read results from disk to reduce file IO operations on consecutive queries. |
| Prettify | True/False | Save files prettified (formatted). Primarily for debugging.                       |
