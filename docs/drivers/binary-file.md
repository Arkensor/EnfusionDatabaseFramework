# BinaryFile
Local `.bin` files for workbench development and *small* data volumes. As compact as possible in terms of file size. **Do not use** for high-frequency complex queries on larger collections. Can however scale infinitely with the same speed on find by ID-only queries. Consult query performance metrics and if causing frame time problems consider switching to a "real" database via `Http` proxy.

### Implementation: [`EDF_BinaryFileDbDriver`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/LocalFile/EDF_BinaryFileDbDriver.c;7)

### Aliases: `BinFile`, `Bin`

### ConnectionInfo: [`EDF_BinaryFileDbConnectionInfo`](https://enfusionengine.com/api/redirect?to=enfusion://ScriptEditor/Scripts/Game/Drivers/LocalFile/EDF_BinaryFileDbDriver.c;2)
| Option   | Values     | Description                                                                       |
|----------|------------|-----------------------------------------------------------------------------------|
| Cache    | True/False | Cache read results from disk to reduce file IO operations on consecutive queries. |
