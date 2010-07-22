// The following identifier turns the IE conditional pre-compilation on.
/*@cc_on @*/

/**
 * @namespace Parent namespace for the api.ovi.com project.
 */
if (typeof ovi == 'undefined') {
    ovi = {};
};

(function (ovi) {
// ovi = function() {
    var that = {};
    
    ovi.description = {
            proxy_url: window.location.protocol + "//" + window.location.host + "/ovi-api",
            sourceLocation: "/js",
            urlSeperator: "/",
            libFileExtension: ".js",
            minified: false,
            sign_method : "method-delegate-oauth",
            env_path : "/alpha",
            dispatch_path : "/api",
            proxy_prefix : "", //useful during dev, so same consumer key can be reused for multiple instances of ovi-api
            request_timeout : 30, // Network request timeout lapse (in seconds).
            caller_id : "", //random id used for caller authentication
            env_device : true // Do not change anything in this line, even the spacing. Else the project won't build properly for SPB.
    };
    // that.description = description;
    
    //Ignore console calls, in case it is not imported
    ovi.console = {init:function(){},log:function(){},error:function(){},debug:function(){},info:function(){}}; 

    //Ignore config calls, in case config.js is not imported
    ovi.config = {setenv:function(){},setServiceEndPoint:function(){}}; 

    var _loaded = {}; //Mapping from script source URL to status (0 - not loaded, 1 - loaded)
    var _libMappings = {}; //Mapping from script source URL to libName 
    var _pending_callbacks = []; //Used to delay callbacks, until all dependencies are loaded 

    /**
     * Method to initialize source URL using location of path from where oviapi.js was loaded
     * 
     */
    var _initSourceURL = function() {
        var scripts = document.getElementsByTagName('script');
        if (!scripts) return; // TODO Throw an exception instead.
        
        var srcFilePattern = new RegExp("/js/ovi/(oviapi|oviapi-min|oviapi_(.+)|oviapi_(.+)-min).js$", "gi");
        for(var index = 0; index < scripts.length; index++) {
            
            var ssrc = scripts.item(index).src;
            if (typeof ssrc !== 'undefined') {
                ssrc = ssrc.replace(/^\s*|\s*$/g,''); // To remove any white spaces.
                
                var startIndex = ssrc.search(srcFilePattern);
                if (startIndex > 0) {
                    
                    // To determine whether the minified version of ovi libraries are used.
                    if (ssrc.indexOf("min") > 0) {
                        ovi.description.minified = true;
                    }
                    
                    // If only relative src can be accessed, then use window.location ro create sourceURL                    
                    if (ssrc.substr(0,6) !== 'http:/' && ssrc.substr(0,7) !== 'https:/') {
                        var rootdir = window.location.pathname.split('/');
                        if (rootdir.length > 1) 
                            ovi.description.proxy_url = window.location.protocol + "//" + window.location.host + "/" + rootdir[1];
                        break;
                    } else {
                        ovi.description.proxy_url = ssrc.substring(0, startIndex);
                        break;
                    }
                }
            }
        }
    };
    _initSourceURL();
     
    /**
     * Get an array of the loaded libs 
     * @param {Integer} Optional param to specify the status of the loaded library.
     * @return Array of libNames
     */
    var _getLoadedLibNames = function(status){
      var loadedLibs = [];  
      if (typeof status === 'undefined') status = -1; 
      for (var item in _loaded){
          if ((status === -1) || (_loaded[item] === status)){
              loadedLibs.push(_libMappings[item]);
          }
      }
      return loadedLibs;
    };
    
    /**
     * Get an object with loaded lib name as key and load status as value (1 - loaded, 0 - not loaded) 
     * @param {Integer} Optional param to specify the status of the loaded library.
     * @return Object with libNames as key and load status as value
     */
    var _getLoadedLibNamesAsObject = function(status){
        var loadedLibs = {};  
        if (typeof status === 'undefined') status = -1; 
        for (var item in _loaded){
            if ((status === -1) || (_loaded[item] === status)){
                loadedLibs[_libMappings[item]] = _loaded[item];
            }
        }
        return loadedLibs;
    };
    
    /**
     * Test if script was already loaded
     * @param {String} scriptSrcURL source URL of script
     * @return {Boolean} 
     */
    var _isExistingScript = function(scriptSrcURL){
        for (var item in _loaded){
            if (scriptSrcURL === item){
                return true;
            }
        }
        return false;
    };
    
    /**
     * Method to verify if a library object has already been loaded. In case of packaged/minified 
     * version of ovi libraries all the libraries would reside in one single JS file.
     * 
     * @param {String} libraryName Name of the library object to check.
     * @return {Boolean} Based on whether the check is true/false. 
     */
    var _isExistingLib = function(libraryName) {
        for (var lib in ovi.libName) {
            if (libraryName == lib) {
                return true;
            }
        }
        return false;
    }
    
    var _determineScriptSrc = function(libName, options){
        var fileName = libName;
        var scriptSrcURL = ovi.description.proxy_url;
        
        // This helps while using this on a widget.
        if (options != undefined && options.sourceURL != undefined) {
            scriptSrcURL = options.sourceURL;
        }
        
        var fileExt = "";
        // If not using slash path, then use ".js" file extension and replace dot with slash.
        if(fileName.indexOf('/') == -1){
            fileExt = ovi.description.libFileExtension;
            fileName = fileName.replace(/\./g,'/');
        }
        
        var rootDir = "";
        //If ovi package, then use source location as root dir
        if (fileName.substr(0,3) === "ovi")
            rootDir = ovi.description.sourceLocation;
        
        scriptSrcURL = scriptSrcURL + rootDir + "/" + fileName 
                            + ((ovi.description.minified) ? "-min" : "") 
                            + fileExt;
        return scriptSrcURL;
    };
    
    var _loadScript = function(scriptSrcURL, srcLib, options){
        
        function _loadedcallback(){
            
            //console.log(new Date().getTime()+" Loaded script:"+scriptSrcURL+" "+_getLoadedLibNames(1));
            //console.log("not loaded:"+_getLoadedLibNames(0));
            
            //Mark script as loaded
            _loaded[scriptSrcURL] = 1;
            
            //If callback exists, then add to pending callbacks
            if (options && options.callback != null)
                _pending_callbacks.push(options.callback);
            
            //If 1 or more items are pending, then don't call callbacks
            var done = true;
            for (var item in _loaded){
                if (_loaded[item] === 0){
                    done = false;
                    break;
                }
            }

            //If all libs are loaded, then call callbacks for all includes, if any
            if (done == true){
                //console.log(new Date().getTime()+" DONE AT LAST\n");
                //Call all pending callbacks
                for (var index = 0; index < _pending_callbacks.length; index++){
                    try{
                        _pending_callbacks[index].call();
                    }catch(err){
                        //console.log("failed to exec callback");
                    }
                }
                _pending_callbacks = []; //Reset pending callbacks list
                ovi.onReady(_getLoadedLibNamesAsObject(1)); //call onReady
            }
        }
        
        // If the library is already loaded (throug packaged/minified source file) then skip the download process.
        if (!_isExistingLib(srcLib)) {
            var scriptTag = document.createElement('script');
            scriptTag.type = 'text/javascript';
            scriptTag.src = scriptSrcURL; 
            
            if(window.opera){
                scriptTag.onload = function(){
                    scriptTag.onload = null;
                    _loadedcallback();
                };
            }
            //From Nicholas C. Zakas (http://www.nczonline.net/blog/2009/06/23/loading-javascript-without-blocking/)
            else if (scriptTag.readyState){  //IE
                scriptTag.onreadystatechange = function(){
                    if (scriptTag.readyState == "loaded" || scriptTag.readyState == "complete"){
                        scriptTag.onreadystatechange = null;
                        _loadedcallback();
                    }
                };
            } else {  //Others
                scriptTag.onload = function(){
                     _loadedcallback();
                };
            }
            
            var documentHead = document.getElementsByTagName("head")[0];
            var newScriptTag = documentHead.appendChild(scriptTag);
            if (newScriptTag) return true;
        } else {
            _loadedcallback();
            return true;
        }
        return false;
    };
    
    /**
     * ovi.include - Method to dynamically include JS libraries from "api.ovi.com".<br/><br/>
     * The ovi.onReady(libs) function is triggered once the library and its
     * dependencies are loaded. See documentation of ovi.onReady.
     * 
     * @param {String,Array}
     *            libNames Names of one or more libraries to be included, specified as an 
     *            array of strings or as a comma separated string.
     *            Library name can be specified in dot notation or as a relative path. <span
     *            style="color:blue">e.g., "ovi.api.people",
     *            "ovi/api/people.1.0.js".</span> External libraries must start
     *            with "lib" <span style="color:blue">e.g., "lib.jquery",
     *            "lib/jquery.json.js".</span>
     * @param {Object}
     *            options Name/Value object to hold other optional parameters.
     * @param {Function}
     *            options.callback Function to be called after a library has
     *            been loaded.
     * 
     * @example 
     *          ovi.include("ovi.api.maps");
     *          ovi.include("ovi.api.people",{callback:function(){callMeBack()}}); 
     *          ovi.include("ovi.api.people, ovi.api.share", {callback:function(){callMeBack()}});
     *          
     * 
     * 
     * @function
     * @methodOf ovi
     */
    ovi.include = function(libNames, options) {
        var importStatus = true;
        try {
            if (libNames != undefined) {
                if (typeof libNames === 'string')
                    libNames = libNames.split(',');
                
                // Determine which scripts (and also libraries) need to be loaded
                var toLoadScript = [];
                for (var index = 0; index < libNames.length; index++){
                    var libName = libNames[index].replace(/^\s*|\s*$/g,''); // Remove any white spaces.
                    
                    // Determine script source URL from libName
                    var scriptSrcURL = _determineScriptSrc(libName, options);
                    if (!_isExistingScript(scriptSrcURL)) {
                        _loaded[scriptSrcURL] = 0;  //Mark script as pending
                        _libMappings[scriptSrcURL] = libName;
                        toLoadScript.push(scriptSrcURL);
                        
                    } else {
                        if (options && options.callback) 
                            toLoadScript.push(scriptSrcURL);
                    }
                    //console.log("o================o");
               }

               var lstatus;
               //Load scripts that need to be loaded. Pass optional callback only for last script (since callback should be called only once).
               for (var index = 0; index < toLoadScript.length; index ++){
                   if (index == (toLoadScript.length-1)) // if last lib, then pass optional callback
                       lstatus = _loadScript(toLoadScript[index], _libMappings[toLoadScript[index]], options);
                   else
                       lstatus = _loadScript(toLoadScript[index], _libMappings[toLoadScript[index]]);
                   if (!lstatus) importStatus = false; 
               }
            }
        } catch (exception) {
            importStatus = false;
        }
        return importStatus;
    };
    // that.include = include;
  
    /**
     * ovi.onReady - Callback function triggered after calling ovi.include,
     * once the library and its dependencies are loaded. Developers can override the default
     * implementation, with their own ovi.onReady function. If successive calls to ovi.include are made,
     * ovi.onReady may be triggered only once.
     * 
     * @param {Object}
     *            libs - library name as key and load status as value (0 - not
     *            loaded, 1 - loaded). Developer must check if the libraries
     *            they need are loaded before using them using the ovi.testIfLoaded function.
     * 
     * @function
     * @methodOf ovi
     */
    ovi.onReady = function(libs) {
        //default, do nothing
        //console.log("onReady:");
        //console.log(libs);
    };
    // that.onReady = onReady;
    
    /**
     * ovi.testIfLoaded - Utility function to test if the specified lib names are marked as loaded in the specified loaded libs object. 
     *                    Used to test the status of libs in ovi.onReady. 
     * 
     * @param {String,Array}
     *            libNames Names of one or more libraries to be included, specified as an 
     *            array of strings or as a comma separated string.
     * 
     * @param {Object}
     *            loadedLibs A mapping of lib name string to load status (0 - not loaded, 1 - loaded)
     * 
     * @return {Boolean}
     * 
     * @function
     * @methodOf ovi
     */
    ovi.testIfLoaded = function(libNames, loadedLibs){
        var reqdlibs = libNames;
        if (typeof libNames === 'string')
            reqdlibs = libNames.split(',');
        for (var index = 0; index < reqdlibs.length; index ++){
            var libName = reqdlibs[index].replace(/\s/g,'');
            if ((libName in loadedLibs) && (loadedLibs[libName] == 1))
                continue;
            else
                return false;
        }
        return true;
    };
    // that.testIfLoaded = testIfLoaded;
    
    /**
     * ovi.setCallerId - Sets caller id required to authenticate API caller with Ovi API backend.
     * 
     * @param {String} 
     *          cid - Caller id string
     *          
     * @function
     * @methodOf ovi                   
     */
    ovi.setCallerId = function(cid){
        ovi.description.caller_id = cid;
    };
    
    // return that;
// }();
})(ovi);
    

/**
 * @property {Object} services Object that holds configurable parameters of all the libraries.
 * @memberOf ovi
 */
ovi.services = {};
ovi.services["import"] = ovi.description;

/**
 * @property {Object} libName Object that holds the identified name of all the OVI API libraries.
 * @memberOf ovi
 */
ovi.libName = {};
ovi.libName["ovi.oviapi"] = ovi;

ovi.include('ovi.config');

/*
@@todo:

- if one include inside of another fails to load, then failure is silent
- file imports will fail silently if the path is wrong
- Safari reinclude doesn't work (seems that DOM removeChild doesn't really remove the script)
*/