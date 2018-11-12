# IceTea
I already tried implementing a build tool before: https://github.com/IngwiePhoenix/IceTea
...but I am not too happy with how it turned out and I hit the limitations of the scripting language used ((ObjectScript)[https://github.com/unitpoint/objectscript]). So I decided to start over and give this whole idea a re-try!

## Why I make this.
Frankly speaking, I want to learn more on C and C++, so I need a project on which I can try out many, many things that I need during such development. Thus, I am rather unhappy with build tools like CMake, GYP or Premake. Therefore, I want to implement my own version of it, which is leant very much to Google's (GN)[https://chromium.googlesource.com/chromium/src/tools/gn/+/48062805e19b4697c5fbd926dc649c78b6aaa138/README.md] - without being tied to any source tree.

Further, I am very inspired by (NPM)[https://npmjs.com] and NodeJS in general. So, why not bring together a proper build tool AND a package manager? I know, it sounds a little bit weird at first, as a full-fledged scripting language and a package manager is the basics of modern platforms such as PHP and Composer, Node and NPM, Ruby and Gems or Python and PIP. Though, tracking dependencies for C and C++ projects is not as easy as it is with tools used by previously named scripting languages. Thar is why I want to finally make this at least a little easier.

Mainly, at some point, I want to resume work on (Phoenix Engine)[https://github.com/Deskshell-Core/PhoenixEngine] - for which I will need a build tool that will allow me to escape the borders of being stuck to just one tool. I mean, combining projects that are either maintained in Scons, CMake or GYP into one bigger project is quite hard. Therefore, Icetea will have the capabilities of doing exactly that as well.

## The scripting language
Previously I used ObjectScript - but now, I want to use (Wren)[http://wren.io]. Wren is maintained more actively than ObjectScript, has a couple more configuration options and features some interesting features such as dedicated `import` statements, stricter classing (and no prototyping) as well as still having things such as operator overloading and other syntactic shugar. Thus, the ObjectScript implementation relied on a very, very hacky approach to realize a desired syntax:

```
// ObjectScript:
function X(head) {
  return function(body) {
    // Actual function implementation here.
  }
}

X("head") {
  // Body
}

// Wren:
var X = Fn.new {|head, body|
  // Actual implementation
}
X("head") {
  // Body
}

// Alternative for Wren, and actually how one should do this:
class MyClass {
  static X(head, body) {
    // Actual implementation
  }
}
MyClass.x("head") {
  // Body
}
```

As you can see, the ObjectScript approach is actually a hacked version: If a function only requires one argument, one can leave out paranthesis and supply the argument right away. Therefore I was able to pass callback functions or maps directly - and the syntax of the actual use is similar to the one implemented in Wren. However, in Wren, I do not need to populate the call stack too much and immediately get what I really want. Thus, if a function takes no arguments, I can leave out the `|argList|`. Unfortunately, object maps are a bit of a different situation here - they basically are JSON objects with all their strictness:

```
// Wren
var x = {
  "Name": "Ingwie"
  Version: "1" // <- Invalid: Key must be quoted, and comma was missing.
}
```

But, there are proper work-arounds for that. After all, IceTea relies on callback functions to build up scopes and configure objects anyway...

## The internal structure
What fascinates me about the DOM is, how we can store complex data in a very nicely accessible tree. Therefore, IceTea will implement what I call `IT-DOM`. That is basically an implementation of the DOM, close to the standard, but dedicated for a whole different use:

* Some types act as branches: Namespaces, for instance.
* Most classes act as elements: Target, Action, etc.
* Looking up and navigating the tree of information is supposed to be as easy as with the DOM.

For instance, the following is supposed to be identical:
```
namespace("IngwiePhoenix") {|ns|
  ns.addTarget("IceTea") {|t|
    t.input = ["..."]
    t.on("finalize") {
      // Callback for when this target is done building.
    }
  }
}

// Versus:
var dom = IceTea.DOM
var target = Target.new("IceTea")
target.input = ["..."]
target.on("finalize") { /* ... */ }
var ns = Namespace.new("IngwiePhoenix")

var root = dom.getRoot()
root.addChild(ns)
ns.addTarget(target)
```

The global function `namespace` is automatically available, whilst the `Namespace` class is meant to be imported for direct usage at another point.

Each target is supposed to live in a namespace, but a global `target` function may be used to attach directly to the root element, as to allow tiny build descriptions like this one:
```
target("miniz", "exe") {|t|
  t.input = ["./*.c"]
}
```

This would create a target that builds into an executable out of all the C source files in the current folder. As such, this would be essentially the most minimal build definition that one could possibly do. A more elaborate example of this would be:
```
import "IceTea/Toolchain/Default" for NativeToolchain
NativeToolchain.Executable("miniz") {|t|
  t.input = ["./*.c"]
}
```

The use of a toolchain's creation methods might be more elaborate, but depending on the toolchain itself, this may not be required and just simply setting the "rule" parameter for `target` would be enough. This is the case for the native toolchain, as it does not attach extra settings or other things. But, what if you are cross-compiling? IceTea is meant to even allow that.

## Toolchains
IceTea is meant to be based on toolchains. The default one is a toolchain that will automatically detect the environment and select the first-found tools, or use environment variables like `$CC` to pick up tools to use. But, there might be other toolchains too to cross-compile for other platforms. For reducing complexity and allowing one target to run for multiple platforms, it may be favourable to create a base target and make copies of it for several toolchains.

```
import "MyCustomToolchain" for MyCustomBinary
import "OtherToolchain" for OtherToolchainBinary
import "IceTea" for Target

namespace("Test") {|ns|
  var baseTarget = Target.createBase("MyTarget")
  baseTarget.input = [ /*...*/ ]

  ns.addTarget(MyCustomBinary.fromBase(baseTarget))
  ns.addTarget(OtherToolchainBinary.fromBase(baseTarget))
}
```

Now, we have two targets - one for each toolchain. Depending on their respective checks, they may decline definitions, and therefore will not actually create a target. Thus, even if the two would allow definitions, they would have to modify the ID of the target. There is a readable, human-friendly name, and an ID that can be used to reference targets. So, for the above example, we may prefix or suffix a target's ID:
```
domRoot
| Namespace "Test"
| | Target "MyCustomBinary:MyTarget"
| | Target "OtherToolchain:MyTarget"
```

Of course, depending on the toolchain, we may even see prefixes that are actual platform tripples:
```
domRoot
| Namespace "Test"
| | Target "apple-darwin16-x86_64:MyTarget"
```

## Externals
As mentioned above, I want IceTea to be able to actually utilize other build tools in situations where porting would either take too long or would be inconvenient. Therefore, here is an example of doing that with cURL - well, a theoretical example, at least.
```
import "stlplus/subsystem" for SyncProcess
import "fs" for File
namespace("curl") {|ns|
  var t = Target.asExternal("curl") {|t|
    t.name = "cURL"

    // Run the configuration
    t.configure {
      if(File.exists("./deps/curl/configure")) {
        var proc = SyncProcess.new(["./deps/curl/configure"])

        // We could add arguments from the CLI here, but we'll skip that now.
        var res = proc.run()

        // Check the return status and return accordingly.
        if(res.exitCode == 0) {
          // Configuration succeeded
          return true
        } else {
          // Configuration failed
          return false
        }
      }

      // Execute the build.
      // Normally, IceTea runs in paralell, but external builds will always
      // be prioritized and run alone, so that the underlying tool may
      // utilize all the available CPU cores.
      t.build {
        var res = SyncProcess.new("make -C ./deps/curl").run()
        if(res.exitCode == 0) {
          return true
        } else {
          return false
        }
      }

      // There are also other methods available:
      t.clean ...
      t.needsRebuild ...
      t.install ...
      t.uninstall ...
      t.package ...

      // We can also export settings for other dependants:
      t.export {|d|
        // "d" is now the dependant target, meaning you can modify it here
        // with additional flags. For instance, this:
        d.settings.addIncludeDir("./deps/curl/include")
      }
    }
  }
  ns.addTarget(t)

  // Allows the user to specify the default target, if IceTea is invoked in
  // the same folder as this very file. This allows the user to just run
  // $ icetea
  // and have this target built automatically.
  ns.setDefaultTarget(t)
}
```

And now, we could either just build cURL via IceTea at the same folder as this file woul be placed, or we could depend on it:
```
namespace("IngwiePhoenix") {|ns|
  ns.addTarget("test") {|t|
    // The slash there just means that IceTea should start looking for
    // the target starting at the root
    t.include("/curl/curl")

    // ... Do other stuff.
  }
}
```

This mechanic allows for non-IceTea projects to receive a lightweight port that will just help other users utilize the project, without having to care for the underlying build tool - ergo, it's an abstraction.

## IceTea is Wren, basically...
Wren does not have the biggest ecosystem yet, but it may grow at some point. And since Icetea utilizes the plain Wren core, it is easily possible to add more modules to it. In fact, IceTea will search not just an `icetea_modules` folder, but also a `wren_modules` one too. IceTea itself will only populate and download content into an `icetea_modules` folder, but should there ever be more Wren modules, it will at least search for them too.

## Testing
I also plan to install an integrated testing framework into IceTea. It may look like so:
```
Test("Suite Name") {|suite|
  suite.beforeEach {
    // Before tests start, call this
  }
  suite.afterEach {
    // The same as above, but for after a test instead.
  }

  suite.describe("A basic test") {|it|
    it("should not be false") {|expect|
      expect(troe).toNotBe(false)
      expect(true).toNotEqual(false)
    }
  }
}
```

## Integration into other package managers
What I also would like to do at some point, is to allow IceTea to integrate into other package managers. Take this JSON file for a Node native add-on for example:
```json
{
  "name": "Test",
  "version": "0.0.0",
  "scripts": {
    "build": "node -e 'require(\"icetea\").run(\"build\")'"
  },
  "dependencies": {
    "icetea": "0.1.0"
  }
}
```

This essentially is meant to allow native add-ons or other projects to utilize IceTea for their building, as it offers more elaborate and safer environment checks.

## IceTea is not just for C/C++ things!
Yes, IceTea is mainly made with native stuff in the back of my head - but, not just! Consider this one:
```
import "PHPToolchain" for PHP
namespace("Test") {|ns|
  var myphar = PHP.PHAR("MyPhar") {|t|
    t.input = ["./*.php"]
  }
  ns.addTarget(myPhar)
}
```

This would just create a PHAR out of all PHP files. And, depending on other targets like that is also possible, should they export their settings properly. IceTea is meant to be used across languages and across systems. It is meant as a general-purpose build tool with a bunch of stuff built-in that will make the process of creating your build as easy as possible. And fast, too.
