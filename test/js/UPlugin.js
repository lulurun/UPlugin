var gPluginsPage = 'http://openugai.wolfdrawer.net/installer/UPluginSetup.exe';
var gLoadingGIF = '<img src="http://openugai.wolfdrawer.net/img/ajax-loader.gif" />';

function UPluginCallback(cb_func, listener_params)
{
    this._cb = cb_func;
    this.exec = function(caller_params) {
	this._cb(caller_params, listener_params);
    };
}

function GetBrowserType() {
    var OsObject = "";
    if(navigator.userAgent.indexOf("MSIE")>0) {
	return 1;
    }
    if(isFirefox=navigator.userAgent.indexOf("Firefox")>0){
	return 2;
    }
    if(isSafari=navigator.userAgent.indexOf("Safari")>0) {
	return 2;
    } 
    if(isCamino=navigator.userAgent.indexOf("Camino")>0){
	return 2;
    }
    if(isMozilla=navigator.userAgent.indexOf("Gecko/")>0){
	return 2;
    }
    return 1;
}

function UPlugin(name, dll_name) {
    this._parent_node = null;
    this._obj = null;
    this._name = name;
    this._dll_name = dll_name;
    this._object_id = function() {
	return 'embed_object_of_UPlugin_' + this._name;
    };
    this._parent_id = function() {
	return 'div_parent_of_UPlugin_' + this._name;
    };
    this._tag_FF = function() {
	return '<embed ' +
	'id="' + this._object_id() + '" ' +
	'type="application/x-uplugin" ' +
	'PluginsPage="' + gPluginsPage + '" width="0" height="0" ' +
	'UPlugin.Name="' + this._name + '" ' +
	'UPlugin.DLLName="' + this._dll_name + '" ' + '>' +
	'</embed>';
    };
    this._tag_IE = function() {
	alert('IE is not supported yet');
	return '';
    };
    this.CreateAppObj = function() {
	this._parent_node = document.createElement("div");
	this._parent_node.setAttribute("id", this._parent_id());
	document.body.appendChild(this._parent_node);
	var parent = $('#' + this._parent_id());
	if (GetBrowserType() == 1) {
	    // IE
	    alert("IE is not supported yet");
	    return;
	} else {
	    parent.html(this._tag_FF());
	}
	this._obj = document.getElementById(this._object_id());
	return this._obj;
    };
    this.DestoryAppObj = function() {
	if (this._parent_node != null) {
	    document.body.removeChild(this._parent_node);
	    this._parent_node = null;
	    this._obj = null;
	}
    };
    this.Obj = function() {
	return this._obj;
    };
};

