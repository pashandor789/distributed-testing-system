# TAsk BAtcher Storage COordinator. (TABASCO)

Service, which stores items for building/executing submissions and bathes of tests.

Nowaday, we have gRPC variant for inner usage, and http variant server for requests from client.

## API

`Request`
```
POST /uploadTests
Content-Type: multipart/form-data; 
{
    Content-Disposition: ... filename="{i}_TEST_FILE_SUFFIX"
    content
    ...
    Content-Disposition: ... name taskId
    taskId
}
```
`Response`
```
200 - OK
400 - BAD REQUEST FORMAT
```
---
`Request`
```
POST /uploadInitScript
{
    scriptName: <string>
    content: <string>
}
```
`Response`
```
200 - OK
400 - BAD REQUEST FORMAT
```
---
`Request`
```
POST /uploadExecuteScript
{
    scriptName: <string>
    content: <string>
}
```
`Response`
```
200 - OK
400 - BAD REQUEST FORMAT
```
---
`Request`
```
POST /createBuild
{
    buildName: <string>
    executeScriptId: <uint64>
    initScriptId: <uint64>
}
```
`Response`
```
200 - OK
400 - BAD REQUEST FORMAT
```