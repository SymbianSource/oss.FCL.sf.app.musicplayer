//Logging for QT

// Override the default alert function.
alert = function(str) {
    console.log(str);
    if(window['context'] && context['owner']){
        context.owner().debugJs('alert: ' + str);
    }
    window.context.owner().errorHandler("Alert", str);
}

function logme(str){
    console.log(str);
    if(window['context'] && context['owner']){
        context.owner().debugJs('debugJs: ' + str);
    }
}

function traceme(){
    var cf = arguments.callee.caller;
    logme('-------------- Start traceme ' + new Date().toLocaleTimeString() + ' --------------');
    var arr = new Array();
    while (cf) {
      var fn = cf.toString();
      fn = fn.substr(0,fn.indexOf('{'));
      arr.push(fn);
      cf = cf.caller;
    }
    //arr.reverse();
    for(i=0;i<=arr.length -1;i++){
        logme(arr[i]);
    }
    logme('-------------- End traceme --------------');
}


window["music"] = window["music"] ||
{};

(function(){
    if (typeof window.context === 'undefined') {
        window['context'] = {
            token: "hipsu:hipsupass",
            song: "Queen - Bohemian Rhapsody",
            objectReservedLength: function(){return 70;},
            objectContent: function() { return "http://music.ovi.com/ru/ru/pc/Product/Queen/Bohemian-Rhapsody/8019069"; },
            objectType: function(){ return "URI"; },
            artist: function() { return "&#9835; Queen"; },
            title: function() { return "Bohemian Rhapsody"; },
            username: function() { return "hipsu"; },
            password: function() { return "hipsupass"; },
            albumArtBase64: function() { return "https://secure-profile.facebook.com/v227/1739/21/n100000756794479_1863.jpg"; },
            owner: function() {
                return {
                    errorHandler: function(a,b) {},
                    clearCache: function() {},
                    debugJs: function(s) {}
                }
            }
        }
    }else{
       try {
           logme(context.objectType() + ' | ' + window.context.objectContent() + ' | ' + window.context.objectReservedLength());
       }catch (e) {
           logme(e);
       }
    }

	var musicContext = null;
	var publishingPlayer = null;
    
    music.getPostContent = function(){
        traceme();
        return {
            reservedLength : window.context.objectReservedLength(),
			type : window.context.objectType(), 
		    content : window.context.objectContent()
        };
    }
    	
    // Hard code something for the browser..

	/**
	 * Check if player is loaded and call error function if not.
	 */
	music.playerLoaded = function() {
        traceme();
		if (!_playerReady()) {
            alert("Error loading player scripts");
		}
	}

	/**
	 * Call this for player to show UI and initialize connection to SNC
	 */
	music.initPlayer = function() 
	{	
        traceme();
	    // Initialize player    
	    publishingPlayer = new ovi.player.publish.Player({
	        locale : "en-US",
	        credentials : {
	            type : "",
	            sso_base_url : "",
	            token : _getNoa() 
	        }
	    });
	    
	    // Show UI 
	    publishingPlayer.create(null, function(status, data){
            traceme();
			if (status != publishingPlayer.status.updateservices_ok && status != publishingPlayer.status.show_ok) {
				// Delegate errors to native side
                window.context.errorHandler(status, data.message);
			} 		
		});	
		
	}
	
	/** 
	 * Call this for player to update context area (the song UI).
	 */
	music.updateContextArea = function() {
		// Update artist and title metadata.
        traceme();
	    publishingPlayer.setContextObject(_getMusicContext());
	}
	
	/**
	 * Call this function to update metadata (called when music player has retrieved the link) 
	 */
	music.updateMetadata = function() {
	    // Update url at bottom of comment field.
        traceme();
		musicContext.updateObject(music.getPostContent());
	}
	
	/**
	 * Call this function to release all resources from player.
	 */
	music.teardown = function() {
        traceme();
		publishingPlayer.cancel();
	}
	
	// Private functions, read data from native side..
	function _playerReady() {
        traceme();
        return typeof ovi != "undefined" &&
            typeof ovi.player != "undefined" &&
			typeof ovi.player.publish != "undefinded";
	}
	
	
	function _getMusicContext() {
	    traceme();            
	    musicContext = new ovi.player.contextobject.Player(); 
        
        var attachment = music.getPostContent();
	
        logme('music._getMusicContext() | attachment'); 	 

        logme('music._getMusicContext() | musicContext.create');

        var sharePlayerArtImage = "";
        var sharePlayerArtBase64 = window.context.albumArtBase64();
        if ( sharePlayerArtBase64.length > 0 ) {
            // TODO: this is temporary solution until base64 defect in QT is fixed.
            sharePlayerArtImage = "<img class='cover' src='" + sharePlayerArtBase64 + "' width='74px' height='74px' />";
//            sharePlayerArtImage = "<img class='cover' src='data:image/png;base64," + sharePlayerArtBase64 + "' width='74px' height='74px' />";
        }
        // Create context object
	    musicContext.create({
	        view: "none",
	        data: { 
	            miniview: "<ul class='list info-list'>"
				/*
						// TODO REMOVE, for testing only
						+ "<p>username: " + window.context.username() + "/" + window.context.password() + "</p><BR>" 
						+ "<input type=\"button\" value=\"Clear JS Cache\" onClick=\"window.context.owner().clearCache();\">"
                */
                        + sharePlayerArtImage
						+ "<span class='description'><div class='title'>" + window.context.title() + "</div>"
						+ "<div class='artist'>" + window.context.artist() + "</div></span>"
					    + "</ul>",
	            object: attachment 
	        }
	    });
        return musicContext;
	}
	                        
	function _getNoa() {
        traceme();
	    // temporary solution until SSO token get from QT SSO DE (now we fetch username, password and re-authenticate using api.ovi.com)
	    var token = (window.context.username)? [window.context.username(),window.context.password()] : window.context.token.split(":");
        logme(token[0] + "/" + token[1]);
	    return {
            username: token[0],
	        password: token[1]
	    }           
	}	
})();
