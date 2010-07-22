window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.contextobject = ovi.player.contextobject ||
{};

ovi.player.contextobject.getInstance = function(config) {
    var player = {};
    player._create = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "CREATE_PENDING");
        }
        // TODO: add context object to player store
        // TODO: store options.miniview to be returned with view as HTML
        // TODO: store other properties of options to be returned with view as JSON
        // TODO: return uri (unique id in this player's store) in callback
    };
    player._cancel = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "CANCEL_PENDING");
        }
        // TODO: cancel pending tasks
        // TODO: cancel Player
    };
    player._delete = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "DELETE_PENDING");
        }
        // TODO: delete object defined in options.uri from store
    };
    player._edit = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "EDIT_PENDING");
        }
        // TODO: update object defined in options.uri
    };
    player._pick = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "PICK_PENDING");
        }
        // TODO: return list of all objects, or object defined in options.uri, fork with options.view
    };
    player._view = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "VIEW_PENDING");
        }
        // TODO: return object defined in options.uri, JSON if options.view == "none", HTML if "mini"
    };
    ovi.player._getInstance.call(player, config);
};

ovi.player.contextobject.Player = ovi.player.contextobject.getInstance;
/*
 * Basic namespaces & common Player functions
 */



window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};



/**
 * A function for creating a callback chain
 * @param {Object} options
 * @param {Object} handler
 * @param {Object} context
 * @return {Function}
 */
ovi.player._notImplemented = function(functionId) {
    return function() {
        var status = ovi.player._status;
        handler.call(context, status["NOT_IMPLEMENTED"], null);
        handler.call(context, status[functionId + "_FAILURE"], null); // TODO: return description, too?
    };
};



/**
 * A method for returning an instance of a singleton
 * @param {Object} options A configuration object for the player
 */
ovi.player._getInstance = function(options) {
    // TODO: store the configuration options
    return (function() {
        return (this.instance ||
        (function() {
            this.instance = {
                // Public interface
                cancel: (this._cancel || ovi.player._notImplemented("CANCEL")),
                create: (this._create || ovi.player._notImplemented("CREATE")),
                "delete": (this._delete || ovi.player._notImplemented("DELETE")), // delete is a keyword
                edit: (this._edit || ovi.player._notImplemented("EDIT")),
                pick: (this._pick || ovi.player._notImplemented("PICK")),
                show: (this._view || ovi.player._notImplemented("VIEW")), // show is the same function as view, included in API for compatibility
                sync: (this._sync || ovi.player._notImplemented("SYNC")),
                view: (this._view || ovi.player._notImplemented("VIEW"))
            };
            return this.instance;
        }()));
    }());
};



ovi.player._status = {
    USER_CANCEL: "USER_CANCEL",
    
    CANCEL_PENDING: "CANCEL_PENDING",
    CANCEL_FAILURE: "CANCEL_FAILURE",
    CANCEL_SUCCESS: "CANCEL_SUCCESS",
    
    CREATE_PENDING: "CREATE_PENDING",
    CREATE_FAILURE: "CREATE_FAILURE",
    CREATE_SUCCESS: "CREATE_SUCCESS",
    
    DELETE_PENDING: "DELETE_PENDING",
    DELETE_FAILURE: "DELETE_FAILURE",
    DELETE_SUCCESS: "DELETE_SUCCESS",
    
    EDIT_PENDING: "EDIT_PENDING",
    EDIT_FAILURE: "EDIT_FAILURE",
    EDIT_SUCCESS: "EDIT_SUCCESS",
    
    PICK_PENDING: "PICK_PENDING",
    PICK_FAILURE: "PICK_FAILURE",
    PICK_SUCCESS: "PICK_SUCCESS",
    
    SYNC_PENDING: "SYNC_PENDING",
    SYNC_FAILURE: "SYNC_FAILURE",
    SYNC_SUCCESS: "SYNC_SUCCESS",
    SYNC_CONNECT_PENDING: "SYNC_CONNECT_PENDING",
    SYNC_CONNECT_FAILED: "SYNC_CONNECT_FAILED",
    SYNC_CONNECT_SUCCEEDED: "SYNC_CONNECT_SUCCEEDED",
    SYNC_LOGIN_PENDING: "SYNC_LOGIN_PENDING",
    SYNC_LOGIN_FAILED: "SYNC_LOGIN_FAILED",
    SYNC_LOGIN_SUCCEEDED: "SYNC_LOGIN_SUCCEEDED",
    SYNC_UPDATE_PENDING: "SYNC_UPDATE_PENDING",
    SYNC_UPDATE_FAILED: "SYNC_UPDATE_FAILED",
    SYNC_UPDATE_SUCCEEDED: "SYNC_UPDATE_SUCCEEDED",
    
    VIEW_PENDING: "VIEW_PENDING",
    VIEW_FAILURE: "VIEW_FAILURE",
    VIEW_SUCCESS: "VIEW_SUCCESS",
    
    NOT_IMPLEMENTED: "NOT_IMPLEMENTED"
};
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.publish = ovi.player.publish ||
{};

ovi.player.publish.getInstance = function(config) {
    var player = {};
    player._create = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "CREATE_PENDING");
        }
        // TODO: create UI
        // TODO: get the sub-players, sync them
    };
    player._cancel = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "CANCEL_PENDING");
        }
        // TODO: cancel sub-players
        // TODO: cancel pending tasks
        // TODO: cancel Player
    };
    ovi.player._getInstance.call(player, config);
};

ovi.player.publish.Player = ovi.player.publish.getInstance;
window["ovi"] = (window["ovi"] || {});
ovi.player = (ovi.player || {});
ovi.player.snc = (ovi.player.snc || {});
ovi.player.snc.engine = (ovi.player.snc.engine || {});


(function(){

    var engine = ovi.player.snc.engine;
	
	
	engine.status = {
		SUCCESS : "SUCCESS",
		FAILED : "FAILED",
	}
	
	
	var _callback = function(status, data, callback, context) {
        if (typeof callback == "function") {
            callback.call(context, status, data);
        }
	}
    
    // External functions	
    
    /**
     * Publish the given object to the social networks.
     * @param {Object} object Published data
     * @param {String} object.text Free text
     * @param {String} object.context.artist Artist name
     * @param {String} object.context.title Song title
     * @param {Array} object.networks Array of social networks ids where to publish
     * @param {Object} [callback] Called when publish is ready.
     * @param {Object} [context]
     */
    engine.publish = function(data, callback, context){
        
        // Just concatenate some status text from received data
        var status = data.text;
        
		// Add attached objects (temporarily just append to end of text)
        // TODO : object.type == "URI" ... etc., add as attachment when supported by SNC 		
        if (typeof data.object != "undefined") {
	         // Get URL to song before publishing and to status text
            var link = data.object.content;
            if (link.length > 0) {
                status += " - " + link;
            }
        }
		 
        // Update to services 		
        ovi.activities.updateStatus(status, {networkId: data.networks }, function(response){
            
			if (response.status == "success") {
                _callback(engine.status.SUCCESS, null, callback, context);
			}
			else {
				// TODO : Implement finer error handling
				_callback(engine.status.FAILED, { message : "Status update failed" }, callback, context);
			}										
        });
    }

	/**
	 * 
	 * @param {Object} data
	 * @param {Object} callback
	 * @param {Object} context
	 */
    engine.cancelPublish = function(data, callback, context){
		_callback(engine.status.SUCCESS, null, callback, context);
    }
    
	
	/**
	 * Reload social networks from SNC backend to the local store. Use getServices() to retrieve the 
	 * list of loaded services.
	 * @param {Object} options Object containing Noa account information. Can be user, password or already
	 * autheticated session token to be used. { username : "", password : "" } or { token : "" }
	 * @param {Object} callback
	 * @param {Object} context
	 */
    engine.sync = function(options, callback, context) {

        // TODO : Seperate init and login from sync (we need both for publish also...)

        // Do service didscovery
		
        // 1. Init APIs
        requests.initOviApi(function(response){
        
		    if(response.status != engine.status.SUCCESS) {
				_callback(engine.status.FAILED, response.data, callback, context);
				return;
			}
		
            // 2. Login noa     
            requests.noaLogin(options, function(response){

	            if(response.status != engine.status.SUCCESS) {
	                _callback(engine.status.FAILED, { message: response.message }, callback, context);
					return;
	            }
            
                // 3. Service discover
                requests.discover(function(response){

                    // Store to player store
					store.clear();
					store.append(response.networks);
					store.commit(); 
					
					// Callback function
					if (response.status == engine.status.SUCCESS) {
                        _callback(engine.status.OK, { networks: response.networks }, callback, context);	
					}
					else {
						_callback(engine.status.FAILED, { message: response.message }, callback, context);
					}					
                });
            });            
        });
    }		
	
	/**
	 * Get list of services (social networks). Data is loaded from local 
	 * player store. To synchronize data social networks from SNC call sync().
	 */
	engine.getServices = function() {
		// Get list of services from store
		return store.getList();
	}
	
	/**
	 * Mark service as selected / unselected. Saves the selection to the persistent 
	 * local storage to save data over sessions.
	 * @param {Object} id
	 * @param {Object} enable
	 */
	engine.selectService = function(id, enable) {
		// Select / unselect service in player store
		store.select(id, enable);		
	}
		
	// Player store	
    var STORE_KEY_SERVICES = "ovi.player.snc.services";
    var STORE_KEY_SELECTED = "ovi.player.snc.services.selected";	
	
	/**
	 * Player store implementation. Persists the social networks (services) and selections 
	 * to the HTML5 local storage, which provides over sessions caching.
	 */
	var store = {

		_services : {},
		_selected : {},
				
        /**
         * Add new server or services to the store.
         * @param {Object} service
         */
		append : function(service) {
			if (service.length) {
	            for(var i=0; i < service.length; i++) {
	                var s = service[i];
	                this._services[s.id] = s;      
	            }
			} else {
				this._services[service.id] = service;
			}
		},
		
		/**
		 * Mark service selected or unselected in the store. Also commits changes in 
		 * selections to the store.
		 * @param {Object} id
		 * @param {Object} enable
		 */
		select : function(id, enable) {
			this._selected[id] = enable;
			localStorage.setItem(STORE_KEY_SELECTED, JSON.stringify(this._selected));
		},
		
		/**
		 * Clear services.
		 */
	    clear : function() {
	   	    this._services = {};
	    },
		
		/**
		 * Get list (array) of services in the store.
		 */
		getList : function() {
			// Convert to array
			var res = [];
			for(var o in this._services) {				
				var serv = this._services[o];
				serv.selected = typeof this._selected[o] != "undefined" ? this._selected[o] : false;
				res.push(serv);
			}			
			return res;
		},
		
		/**
		 * Commit services to the store.
		 */
		commit : function() {
			localStorage.setItem(STORE_KEY_SERVICES, JSON.stringify(this._services));
		},
		
		/**
		 * Retrieve services and selections in the store.
		 */
		load : function() {
			this._services = JSON.parse(localStorage.getItem(STORE_KEY_SERVICES));
			this._selected = JSON.parse(localStorage.getItem(STORE_KEY_SELECTED));

			if (this._services == null) this._services = {};
			if (this._selected == null) this._selected = {};			
		}
	}
		
	
    // SNC Request implementation
    var requests = {
    
        /**
         *
         * @param {Object} callback
         */
        initOviApi: function(callback){
            var myincludes = "ovi.auth.noa_login, ovi.api.snc, ovi.api.activities, ovi.net.xss";
            ovi.onReady = function(libs){
                if (ovi.testIfLoaded(myincludes, libs)) {
                
                    // Successful, set environment for NCIM              
                    ovi.config.setenv("st-account", "delegate");
                    
                    // Allow cross-domain scripting
                    function xssInitCallback(data){
                        if (data.event === "InitializeOk") {
                            callback( { status : engine.status.SUCCESS });
                        }
                    }
                    
                    ovi.net.XssInit({
                        back_url: "http://spb.ci.wipsl.com/ovi-api/js/ovi/net/",
                        callback: xssInitCallback
                    });
                    
                }
            }
            
            ovi.include(myincludes);
        },
        
        /**
         *
         * @param {Object} callback
         */
        noaLogin: function(options, callback){
			
			var noa = {};			
			if (typeof options.token != "undefined") {   // This propably is not supported yet..
				noa = { token : options.token };
			}
			else {
				noa = { username : options.username, password : options.password };
			}
			
			// Login
            ovi.noa.login(noa, function(state){
				            
                // Callback
                if (state.status == "success") {
                    callback( {
						status: engine.status.SUCCESS
                    } );
                }
                else {
					callback( {
						status: engine.status.FAILED,
						message: "NOA login failed - " + state.statusCode + " - " + state.status
					} );
                }
            });
        },
        
        /**
         *
         * @param {Object} callback
         */
        discover: function(callback){
        
            // Discover service available
            ovi.snc.discoverSocialNetworks(function(response){
            
                if (response.status != "success") {
                    callback( {
						status: engine.status.FAILED,
						message : "Service discovery failed - " + response.statusCode + " - " + response.responseText 
                    } );
                    return;
                }
				
                var netw = response.data.network;
                
                // And get activated
                ovi.snc.getActivatedSocialNetworks(function(response){                    

                    if (response.status != "success") {
	                    callback( {
	                        status: engine.status.FAILED,
	                        message : "Service discovery failed - " + response.statusCode + " - " + response.responseText 
	                    } );
												
                        return;
                    }
					else if (response.statusCode == 204) { // Not an error, no active networks
                        callback( {
                            status: engine.status.SUCCESS,
                            networks : [] 
                        } );
						
						return;
					}
                    
					var active = response.data.network;
                    
                    // Now we have finally the service discovery ready, create final response										
					// Remove all not-active networks from supported networks					
                    var results = [];					
					for(var i=0; i < netw.length; i++) {
						var id = netw[i].id;
						
						for(var j=0; j < active.length; j++) {
							if (active[j].id == id) {								
								// We have a winner
								results.push({ name : netw[i].name, id : netw[i].id, maxTextInput : netw[i].maxTextInput });
								break;
							}
						}
					}					
                    
                    callback( { 
					   status: engine.status.SUCCESS, 
					   networks : results 
                    } );                    
                });
            });
        }
    }
    
	// Restore data from store
    store.load();
    
})();
// Create needed namespace ovi.player.share.ui
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.share = ovi.player.share ||
{};
ovi.player.share.ui = ovi.player.share.ui ||
{};

/**
 * TODO
 * 
 * This API tries to imitate medos framework UI controls for future compability.
 * @param {Object} params
 * @param {Object} target
 */
ovi.player.share.ui.Button = function(params, target) {
    
	var CLICK_EVENT = "selected";
	var that = this;

    function createHtml() {
	    var template = '<button class="ovi_Button ovi_unselectable ovi_clip">' +
	       params.text + '</button>';
	
	    var node = document.createElement("div");  // Do we need this ??
	    node.innerHTML = template;
		return node;		
	}

    var _root = createHtml();
	var _button = _root.firstChild;
 
    target.appendChild(_root);
	
	// Interface functions
	this.addEventHandler = function(event, callback) {
		// selected = click (comes from medos...)		
		if (event == CLICK_EVENT) {
            _root.addEventListener("click", function(e) { callback.call(that, e); }, that);	
		}		 
	} 	
		
    this.setProperty = function(name, value) {
		_button[name] = value;
	}
	
    this.getProperty = function(name) {
        return _button[name];
    }	
}
// Create needed namespace ovi.player.share.ui
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.share = ovi.player.share ||
{};
ovi.player.share.ui = ovi.player.share.ui ||
{};

/**
 * Checkbox UI control implementation. TODO
 * 
 * This API tries to imitate medos framework UI controls for future compability.
 * @param {Object} params
 * @param {Object} target
 */
ovi.player.share.ui.CheckBox = function(params, target) {
    
	var that = this;

    function createHtml() {
		      
        var template = '<div class="ovi_CheckBox ovi_unselectable ovi_clip">' +
		                  '<input type="checkbox"></input>' +
		                  '<span data-bind-text="label">' + params.label + '</span>' +
					   '</div>';
		
	    var node = document.createElement("div");  // Do we need this ??
	    node.innerHTML = template;
		return node;		
	}

    var _root = createHtml();
	var _checkBox = _root.firstChild.firstChild;
 
    target.appendChild(_root);	
	
    this.setProperty = function(name, value) {
		
		if (name == "selected") { // Use medos property name mappings
            _checkBox["checked"] = value;	
		} 
		else {
			_checkBox = value;
		}        
    }
	
	this.getProperty = function(name) {
		if (name == "selected") {
			return _checkBox["checked"];
		}
		return _checkBox[name];
	}
	
	for(var v in params) {
		this.setProperty(v, params[v]);
	}
}
// Create needed namespace ovi.player.share.ui
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};


/**
 * Context object player poc implementation
 */
ovi.player.contextobject = ovi.player.contextobject ||
{};


ovi.player.contextobject.Player = function() {
	
	var targetNS = this;	
	targetNS.create = function(params) {
		targetNS.object = params;		
	}
}



/**
 * Publish player poc implementation 
 */
ovi.player.publish = ovi.player.publish ||
{};

// Extend the namespace
ovi.player.publish.Player = function(params) {

    // TODO : MAKE THIS SINGLETON SOMEHOW ??
    var targetNS = this;    

    // Utils
    var _id = function(id) {
        return document.getElementById(id);
    };
	
    var _addClass = function(target, className) {
        var classes = target.className;
        if (!classes.match(new RegExp("\\b" + className + "\\b"))) {
            if (classes != "" && classes.substr(-1) != " ") {
                target.className += " ";
            }
            target.className += className;
        }
    };
    var _removeClass = function(target, className) {
        target.className = target.className.replace(new RegExp("\\b" + className + "\\b", "g"), "");
        //TODO: clean extra spaces?
    
    };
    var _toggleClass = function(target, className) {
        if (target.className.indexOf(className) == -1) {
            _addClass(target, className);
            return true;
        } else {
            _removeClass(target, className);
            return false;
        }
        
    };
    
    
    
    /**
     * "State engine"
     */
    var _state = {
        visible: false,
        services: {}
    };
    
    
    
    /**
     * HTML templates for ui
     */
    var _templates = {
        // TODO: localization
        "share.ui": '<div class="player">\
			<div class="panel header">Publish</div>\
			<div class="panel panel-message">\
				<div class="info hidden" id="comment-limit"><span id="comment-length">0</span>/<span id="comment-maxlength">0</span></div>\
				<div class="comment" id="comment-area">\
					<div class="hint">Write message here</div>\
					<textarea id="message"></textarea>\
				</div>\
				<hr/><div class="info" id="shared-object"></div><hr/>\
			</div>\
			<div class="panel networks-panel"><ul class="list checked" id="available-services"></ul></div>\
			<div class="panel panel-buttons">\
				<ul class="list button-bar">\
					<li id="action-ok" class="disabled"><span>OK</span></li>\
					<li id="action-cancel"><span>Cancel</span></li>\
				</ul>\
			</div>\
		</div>'
    };
    
    
    
    /**
     * Generic callback invoker
     *
     * @param {Object} notification
     * @param {Object} data
     * @param {Object} callback
     * @param {Object} context
     */
    var _callback = function(notification, data, callback, context) {
        if (typeof callback == "function") {
            callback.call(context, notification, data);
        }
    };
    
    
    
    /**
     * status messages
     */
    var _status = {
    
        show: "SHOW",
        show_ok: "SHOW_SUCCEEDED",
        show_fail: "SHOW_FAILED",
        
        updateobject: "UPDATEOBJECT",
        updateobject_ok: "UPDATEOBJECT_SUCCEEDED",
        updateobject_fail: "UPDATEOBJECT_FAILED",
        
        updateservices: "UPDATESERVICES",
        updateservices_ok: "UPDATESERVICES_SUCCEEDED",
        updateservices_fail: "UPDATESERVICES_FAILED",
		updateservices_nonetworks : "UPDATESERVICES_NONETWORKS"
    
    };
    
    
    
    /**
     * Invokes the ui for the player
     *
     * @param {Object} [options]
     * @param {Object} [options.target] the dom node or id where the ui should be inserted
     * @param {Object} [options.template] the html for the ui
     * @param {Object} [callback]
     * @param {Object} [context]
     */
    var _show = function(options, callback, context) {
    
        var target = (options && options.target) || _id("ovi.player.share.ui"), template = (options && options.template) || _templates["share.ui"];

        // Find target node if id was given
        if (typeof target == "string") {
            target = _id(_target);
        }
        if (target) {
            if (template) {
                if (_state.visible && target.innerHTML.indexOf('"player"') >= 0) {
                    // _show was already called earlier, and player is shown
                    _callback(_status.show, {
                        message: "Player is already visible"
                    }, callback, context);
                } else {
                    target.innerHTML = template;
                    
                    // add handler for textarea
                    var message = _id("message");
                    _id("message").onkeyup = _handleMessageChange;
                    message.style.backgroundColor = "transparent";
                    
                    // add handlers for buttons
                    _id("action-ok").onclick = _handleSubmit;
                    _id("action-cancel").onclick = _handleSubmit;
                    
                    // Update state
                    _state.visible = true;
                    _callback(_status.show_ok, null, callback, context);
                }
            } else {
                _callback(_status.show_fail, {
                    message: "Template not found",
                    data: {
                        template: template
                    }
                }, callback, context);
            }
        } else {
            _callback(_status.show_fail, {
                message: "Target container not found",
                data: {
                    target: target
                }
            }, callback, context);
        }
		
		onResize();   // Initial resize
		
		// If we would like to show the services stored in local storage, uncommenting the following would do 
		// the job..
		/*
		var services = ovi.player.share.engine.getServices();
		targetNS.updateServices(services);*/ 		
    };
    
    /**
     * Removes all the handlers from the ui and the ui
     *
     * @param {Object} options
     * @param {Object} callback
     * @param {Object} context
     */
    var _teardown = function(options, callback, context) {
        //TODO: clear handlers
        //TODO: destroy html
        //TODO: update status
    };

    
    var _updateServices = function(data, callback, context) {
    
        var target = _id("available-services");
        
        if (target) {
            if (data.length) {
                var listToCheck = [], i;
                // TODO: add loading animation?
                for (i = 0; i < data.length; i++) {
                    var id = data[i].id, label = data[i].name;
                    if (id && label) {
                        id = "service-" + id;
                        listToCheck.push(id);
                        if (!_id(id)) {
                            // We don't have the network in our list yet, create one
                            var item = document.createElement("LI"), checkbox = document.createElement("SPAN");
                            checkbox.setAttribute("class", "checkbox");
                            checkbox.appendChild(document.createTextNode(label));
                            item.appendChild(checkbox);
                            item.setAttribute("id", id);
                            item.onclick = _handleServiceListClick;
                            target.appendChild(item);
                            // store object to the state
							if (data[i].selected) {
                                data[i].checked = _toggleClass(item, "checked");
							}							
                            _state.services[id] = data[i];
                        }
                    } else {
                        _callback(_status.updateservices, {
                            message: "Service object formatted badly",
                            data: data[i]
                        }, callback, context);
                    }
                }
                //make a searchable string
                listToCheck = listToCheck.join();
                //remove obsolete networks
                for (i = 0; i < target.childNodes.length; i++) {
                    var node = target.childNodes[i], id = node.getAttribute("id");
                    if (listToCheck.indexOf(id) == -1) {
                        node.onclick = null;
                        target.removeChild(node);
                    }
                    // TODO: update _state?
                }
				_updateServiceLimits();
                _callback(_status.updateservices_ok, null, callback, context);				
            } else {
                _callback(_status.updateservices_fail, {
                    message: "Received data was not an array",
                    data: data
                }, callback, context);
            }
        } else {
            _callback(_status.updateservices_fail, {
                message: "Target container not found"
            }, callback, context);
        }
    };
    
    /**
     * Handler for message changes
     * @param {Object} e
     */
    var _handleMessageChange = function(e) {
        _id("comment-length").innerHTML = this.value.length;
        _checkMessageLength();
    }
    
    
    
    /**
     * Handler for service selection
     * @param {Object} e
     */
    var _handleServiceListClick = function(e) {
        _state.services[this.id].checked = _toggleClass(this, "checked");
		ovi.player.snc.engine.selectService(_state.services[this.id].id, _state.services[this.id].checked);
        _updateServiceLimits();
    };
    
    
    
    /**
     * Check limits and if the OK button can be clicked
     */
    var _updateServiceLimits = function() {
        var bigNumber = 9999999, maxTextInput = bigNumber, min = Math.min, services = _state.services, service;
        for (sid in services) {
            service = services[sid];
            if (service.checked && service.maxTextInput && service.maxTextInput > 0) {
                maxTextInput = min(maxTextInput, service.maxTextInput);
            }
        }
		
        if (maxTextInput < bigNumber) {
            // Reduce the link and context reserved size from input (data that will be allocated 
			// from maxTextInput for context)
			var contextData = _state.contextPlayer.object.data;
            maxTextInput -= contextData.object.reservedLength;
			     			
            _id("comment-maxlength").innerHTML = maxTextInput;
            _removeClass(_id("comment-limit"), "hidden");
            //TODO: alter size of elements to make space for the limit?
        } else {
            _id("comment-maxlength").innerHTML = maxTextInput;
            _addClass(_id("comment-limit"), "hidden");
        }
        _checkMessageLength();
    };
    
    
    
    var _checkMessageLength = function() {
        var length = parseInt(_id("comment-length").innerHTML), maxLength = parseInt(_id("comment-maxlength").innerHTML), message = _id("message");
        if (length == 0) {
            message.style.backgroundColor = "transparent";
        } else {
            message.style.backgroundColor = "";
        }
        if ((maxLength > 0) && (length > maxLength)) {
            _addClass(_id("comment-limit"), "error");
            _addClass(message, "error");
        } else {
            _removeClass(_id("comment-limit"), "error");
            _removeClass(message, "error");
        }
        _updateActions();
    };
    
    
    
    var _updateActions = function() {
        if (_canSubmit()) {
            _removeClass(_id("action-ok"), "disabled");
        } else {
            _addClass(_id("action-ok"), "disabled");
        }
    };
    var _canSubmit = function() {
        var length = parseInt(_id("comment-length").innerHTML), maxLength = parseInt(_id("comment-maxlength").innerHTML), services = _state.services, service;
        if ((maxLength > 0) && (length > maxLength)) {
            return false;
        }
        for (sid in services) {
            service = services[sid];
            if (service.checked && service.maxTextInput && service.maxTextInput > 0) {
                return true;
            }
        }
        return false;
    }
    
    
    var _handleSubmit = function(e) {
        // don't accept clicks from elements with class "disabled"
        if (/\bdisabled\b/.test(this.className)) {
            return;
        }
        if (this.id == "action-ok") {
            _addClass(this, "disabled");
            _addClass(_id("action-cancel"), "disabled");
            //TODO: disable testarea and service list?
            
            var services = _state.services, service, networks = [];
            for (sid in services) {
                service = services[sid];
                if (service.checked) {
                    networks.push(service.id);
                }
            }
			
			var contextData = _state.contextPlayer.object.data;
			
            var data = {
                text: _id("message").value,				
                object : contextData.object,
                networks: networks
            };
			
            ovi.player.snc.engine.publish(data, function(status, data) {
				if (status == ovi.player.snc.engine.status.FAILED) {
					alert(data.message);
				}
                _reset();
            });
            // TODO: call window.close here? teardown first? window.close in teardown?
        } else {
            ovi.player.snc.engine.cancelPublish(null, function(status, data) {
                _reset();
            });
        }
    };
    
    var _reset = function() {
        _id("shared-object").innerHTML = "";
        _id("message").value = "";
		_handleMessageChange.call(_id("message"));
        _removeClass(_id("action-cancel"), "disabled");
		
		// Request window to be closed
		window.close();		
    };
    
	var _sync = function(contextPlayer, handler, context) {    
        // Sync context to UI
        _state.contextPlayer = contextPlayer; // Store context object for publishing
        _id("shared-object").innerHTML = _state.contextPlayer.object.data.miniview;          
    }	
	
	
	var _sncInit = function(credentials, handler, context) {
        // Sync SNC stuff
        ovi.player.snc.engine.sync(credentials.token, function(status, data) {
            if (status == ovi.player.snc.engine.status.FAILED) {
                _callback(_status.updateservices_fail, 
				    { message: data.message }, 
					handler, context);				    
            }
            else if (data.networks.length == 0) {
                _callback(_status.updateservices_nonetworks, 
				          { message: "No networks" }, 
						  handler, context);
            }
            else {
                var services = ovi.player.snc.engine.getServices();
                _updateServices(services, handler, context); 
            }               
        } );           		
	}
		   


    // Register onResize for landscape / portrait mode changes
    window.addEventListener("resize", onResize, true);
    
    function onResize(e) {
        
        function isLandscape(){
            return window.innerWidth > window.innerHeight;
        }   
		
		function _setMode(mode) {
            _changeMode(_id("comment-area"), mode);
            
            var services = _id("available-services").children;                      
            for(var i=0; i < services.length; i++) {
                _changeMode(services[i], mode);  
            }           			
		}
		        
        // Determine - landscape or portrait mode
        if (isLandscape()) {		
			_setMode("landscape");						
        }
        else {
            _setMode("portrait");			
        }       
    }
	
	function _changeMode(id, mode) {
		if (mode == "portrait") {
            _removeClass(id, "landscape");
			_addClass(id, "portrait");				
		} else {
            _removeClass(id, "portrait");
			_addClass(id, "landscape");              			
		}	   	
	}
	
    /**
     * Assign needed functions to the target namespace => defined public API.
     */
    targetNS.view = _show;
    targetNS.teardown = _teardown;
    targetNS.reset = _reset;    
    
    /**
     * Synchronize context data from context object
     */
    targetNS.sync = _sync;
    
    /**
     * Launch UI and initialize SNC
     */
    targetNS.create = function(options, handler, context) {
        
        // Show UI
        _show(options, handler, context);
        
        // Start loading SNC networks
        _sncInit(params.credentials, handler, context);
    }			
	
	targetNS.status = _status;
};
// Create needed namespace ovi.player.share.ui
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.share = ovi.player.share ||
{};
ovi.player.share.ui = ovi.player.share.ui ||
{};

/**
 * Label UI control implementation. TODO
 * 
 * This API tries to imitate medos framework UI controls for future compability.
 * @param {Object} params
 * @param {Object} target
 */
ovi.player.share.ui.Label = function(params, target) {
    
	var CLICK_EVENT = "selected";
	var that = this;

    function createHtml() {
	    var template = '<span class="ovi_Label">' + params.text + '</span>';
	
	    var node = document.createElement("div");  // Do we need this ??
	    node.innerHTML = template;
		return node;		
	}

    var _root = createHtml();
	var _label = _root.firstChild;
 
    target.appendChild(_root);
	
    this.setProperty = function(name, value) {
		_label[name] = value;
	}
	
    this.getProperty = function(name) {
        return _label[name];
    }
	
    for(var v in params) {
        this.setProperty(v, params[v]);
    }
}
// Create needed namespace ovi.player.share.ui
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.share = ovi.player.share ||
{};
ovi.player.share.ui = ovi.player.share.ui ||
{};

/**
 * Textarea UI control implementation. TODO
 * 
 * This API tries to imitate medos framework UI controls for future compability.
 * @param {Object} params
 * @param {Object} target
 */
ovi.player.share.ui.TextArea = function(params, target) {
    
	var CLICK_EVENT = "selected";
	var that = this;

    function createHtml() {
	    var template = '<textarea class="ovi_TextInput ovi_Textarea" rows="2"></textarea>';
	
	    var node = document.createElement("div");  // Do we need this ??
	    node.innerHTML = template;
		return node;		
	}

    var _root = createHtml();
	var _textarea = _root.firstChild;
 
    target.appendChild(_root);
	
    this.setProperty = function(name, value) {
		_textarea[name] = value;
	}
	
	this.getProperty = function(name) {
		return _textarea[name];
	}
	
    for(var v in params) {
        this.setProperty(v, params[v]);
    }	
}
window["ovi"] = window["ovi"] ||
{};
ovi.player = ovi.player ||
{};
ovi.player.snc = ovi.player.snc ||
{};

ovi.player.snc.getInstance = function(config) {
    var player = {};
    player._cancel = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "CANCEL_PENDING");
        }
        // TODO: cancel Player
    };
    player._pick = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "PICK_PENDING");
        }
        // TODO: offer list of configured networks
    };
    player._view = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "VIEW_PENDING");
        }
        // TODO: offer miniviews for networks list
    };
    player._sync = function(options, handler, context) {
        var callable = (typeof handler === "function");
        if (callable) {
            handler.call(context, "SYNC_PENDING");
        }
        // TODO: connect to the Ovi API and SNC
    };
    ovi.player._getInstance.call(player, config);
};

ovi.player.snc.Player = ovi.player.snc.getInstance;
