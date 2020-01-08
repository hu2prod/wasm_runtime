assert = require "assert"
wasm = require "wasm"
{execSync} = require "child_process"

describe "compile section", ()->
  it "compile", (on_end)->
    @timeout 10000
    execSync "rm lib/build/*.o  2>/dev/null ; echo -n ''"
    
    opt = {
      dir : "lib"
      seq : true
    }
    await wasm.lib_compile opt, defer(err, res); return on_end err if err
    
    on_end()
  
  