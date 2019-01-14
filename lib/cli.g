class CommandLine {
  private var _options;
  private var _struct;
  private var _order;
  private var _manuals;

  class Arguments {
    var argv0;
    var values;
    var defines;
    var words;

    func init() {
      values = {}
      defines = {}
      words = []
    }
  }

  public func init() {
    _options = []
    _struct = []
    _order = []
    _manuals = {}
    _args = []
  }

  /**
   * Parses the command line array given from C.
   *
   * The command line array is expected to be a type at least similiar to
   * Gravity's List.
   *
   * This parser will parse the following:
   *   - `-x`: A single, short option
   *   - `--abc`: A single, long option
   *   - `--abc=def`: A single, long option with a value
   *   - `foo=bar`: A definition. Dot-notation is allowed to create objects,
   *     as well as brackets are allowed for arrays.
   *   - `foo`: A single word. As long as it does not contain an equal sign,
   *     it is treated as a word.
   *   - `-abc`: If `a` takes a value, it will parse `-a` with value `bc`. But
   *     if `-a` does not take a value, it will check if `-b` would - and so on
   *     and so forth. So either it will read `-a bc`, `-ab c`, or `-abc` (where
   *     `-c` will have no value if it needed one) or `-a -b -c`.
   * @type {[type]}
   */
  public func parse(argv) {
    var args = new Arguments()
    args.argv0 = argv[0]

    for(i=1; i<argv.count; i++) {
      var arg = argv[i]

    }
  }

  public func addOption(opt, category="") {}
  public func removeOption(id) {}
  public func create(id, category="") {
    if(!_struct[category]) {
      _struct[category] = []
    }

    // There is an issue where calling a method on a list member...calls a list
    // method instead. So, here is a workaround, for now.
    var currCategory = _struct[category]
    var opt = new Option(id)
    currCategory.push(opt)
    return opt
  }

  public func addManual(name, data) {
    if(!(data isa Manual)) {
      Fib.abort("Second argument is expected to be of type Manual.")
    }
    _manuals[name] = data
  }

  public func hasCategory(category) {
    return new Bool(_struct[category])
  }

  public func hasOption(id) {
    for(i=0; i<_struct.count; i++) {
      var category = _struct[i]
      for(j=0; j<category.count; j++) {
        var opt = category[j]
        if(opt.id == id) {
          return true;
        }
      }
    }
    return false
  }

  public func help() {}
  public func printHelp() {}

  public func usage() {}
  public func printUsage() {}

}

class Option {
  private var _id;

  private var _short;
  private var _long;
  private var _desc;

  private var _valueRequired;
  private var _valueDesc;
  //private var _valueType;
  private var _valueMultiple;
  private var _valueOptional;

  var id {
    get { _id }
    // Do I need a setter? o.o
  }

  var short {
    set {|v| if(v isa String) _short = v }
    get { _short }
  }
  var long {
    set {|v| if(v isa String) _long = v }
    get { _long }
  }
  var desc {
    set {|v| if(v isa String || v isa List) _desc = v }
    get { _desc }
  }

  var valueRequired {
    set {|v| if(v isa Bool) _valueRequired = v }
    get { _valueRequired }
  }
  var valueDesc {
    set {|v| if(v isa String) _valueDesc = v }
    get { _valueDesc }
  }
  var valueMultiple {
    set {|v| if(v isa Bool) _valueMultiple = v }
    get { _valueMultiple }
  }
  var valueOptional {
    set {|v| if(v isa Bool) _valueOptional = v }
    get { _valueOptional }
  }

  func init(id) {
    _id = id
  }

  private func setValue(key, value) {
    self[key] = value
    return self;
  }

  func String() {return toString()}
  func toString(ident=2, leftColumnLength=null) {
    var out = "";
    var prefix = "";

    // Add base ident
    ident.loop({prefix += " "})

    out += prefix
    if(_short && !_long) {
      out += _short
      if(_valueDesc) out += "=" + _valueDesc
    } else if(!_short && _long) {
      out += _long
      if(_valueDesc) out += "=" + _valueDesc
    } else if(_short && _long) {
      out += _short
      if(_valueDesc) out += "=" + _valueDesc
      out += "|" + _long
      if(_valueDesc) out += "=" + _valueDesc
    }

    out += " "

    // Capture the current line length. This is now the new ident to use.
    if(!leftColumnLength) {
      ident = out.count
    } else {
      ident = leftColumnLength
    }

    if(_desc isa String) {
      out += _desc + "\n"
    } else {
      var tmp = desc
      out += tmp[0] + "\n"
      tmp[0] = null
      tmp.loop({|v|
        if(v == null) return; // Skip
        ident.loop({out += " "})
        out += v + "\n"
      })
    }

    return out;
  }

  func getLeftColumnLength(ident=2) {
    var out = 0;
    out += ident
    if(_short && !_long) {
      out += _short.count
      if(_valueDesc) out += _valueDesc.count + 1 // with equal sign
    } else if(!_short && _long) {
      out += _long
      if(_valueDesc) out += _valueDesc.count + 1 // with equal sign
    } else if(_short && _long) {
      out += _short
      if(_valueDesc) out += _valueDesc.count + 1 // with equal sign
      out += _long.count + 1 // with separator
      if(_valueDesc) out += _valueDesc.count + 1 // with equal sign
    }

    // Add initial space after the option itself
    out += 1

    return out;
  }

  func short(val) {return setValue("short", val)}
  func long(val) {return setValue("long", val)}
  func desc(val) {return setValue("desc", val)}

  func required(val) {return setValue("valueRequired", val)}
  func valueDesc(val) {return setValue("valueDesc", val)}
  func multiple(val) {return setValue("valueMultiple", val)}
  func optional(val) {return setValue("valueOptional", val)}
}

class Manual {
  private var _txt;
  var txt {
    set {|v| if(v isa String) _txt = v }
    get { _txt }
  }

  func String() { return _txt; }
  func render() { System.print(_txt); }
}
