var gSpaceAgent = gSpaceAgent || {
    _uplugin_name : 'SpaceAgent',
    _uplugin_dll_name : 'UPlugin.SpaceAgent.dll',
    _uplugin : null,
    _obj : function() {
	if (this._uplugin == null) {
	    return null;
	}
	return this._uplugin.Obj(); 
    },
    STATE : {
	STARTUP : 65536,
	SHUTDOWN : 131072,
	STARTED_NOCONTENTS : 1048576,
	CONTENTS_READY : 2097152,
	LOAD_CONTENTS : 196608,
	LOAD_CONTENTS_30 : 196608 + 20480,
	ERR_START : 33554432,
	ERR_LOAD : 16777216,
	LOCAL_START : -1,
	LOCAL_STOP : -2
    },
    MESSAGE : function(state){
	switch(state){
	case this.STATE.STARTUP :
	  return "Starting up";
	case this.STATE.SHUTDOWN :
	  return "Shutting down";
	case this.STATE.STARTED_NOCONTENTS:
	  return "Started";
	case this.STATE.CONTENTS_READY:
	  return "Space Ready";
	case this.STATE.LOAD_CONTENTS:
	  return "Loading Contents 0%";
	case this.STATE.LOAD_CONTENTS_30:
	  return "Loading Contents 30%";
	case this.STATE.ERR_START:
	  return "Can not Start Space";
	case this.STATE.ERR_LOAD:
	  return "Can not Load Contents";
	case this.STATE.LOCAL_START:
	  return "Waiting";
	case this.STATE.LOCAL_STOP:
	  return "Stop";
	}
	return ". . .";
    },
    // Constants
    _GetStatusURL : "http://127.0.0.1:7475/AgentService/status",
    _status : {code:-100},
    Status : function() { return this._status; },
    StatusCode : function() { return this._status.code; },
    _get_status_timer : null,
    _load_contents_data_uuid : null,
    _get_contents_url : '',
    _inited : false,
    _started : false,
    _onStateChange : null,
    
    init : function(get_contents_url, OnStateChange) {
	if (this._inited) return;
	this._inited = true;
	this._onStateChange = OnStateChange;
	this._get_contents_url = get_contents_url;
	this._uplugin = new UPlugin(this._uplugin_name, '');
	this._uplugin.CreateAppObj();
    },
    destroy : function() {
	this.stop();
	if (this._uplugin != null) {
	    this._uplugin.DestoryAppObj();
	    this._uplugin = null;
	}
	this._get_contents_url = '';
	this._onStateChange = null;
	this._inited = false;
    },
    start : function() {
	if (!this._inited) return;
	this._get_status();
	if (this._started) return;
	this._started = true;
	if (this._get_status_timer == null) {
	    this._get_status_timer = setInterval("gSpaceAgent.get_status_timer()", 500);
	}
    },
    stop : function() {
	if (this._get_status_timer != null) {
	    clearInterval(this._get_status_timer);
	    this._get_status_timer = null;
	}
	this._started = false;
	this._status = {code:-100};
    },
    _get_status : function() {
	var local_status = this._obj().GetStatus();	
	if (local_status == this.STATE.LOCAL_START) {
	    this._set_status({ code:local_status });
	    $.getJSON(this._GetStatusURL + "?jsoncallback=?",
		      function(data){
			  gSpaceAgent._set_status(data);
		      });
	} else {
	    this._set_status({ code:local_status });
	}
    },
    _set_status : function(status) {
	if (status.code == gSpaceAgent.STATE.LOCAL_START) {
	    if (this._status.code != gSpaceAgent.STATE.LOCAL_STOP) return;
	}
	if (this._status.code != status.code && this._onStateChange != null) {
	    this._onStateChange.exec(status);
	}
	this._status = status;
    },
    start_space : function() {	
	if (!this._inited) return false;
	if (!this._obj().Run()) {
	    alert("start failed");
	    return false;
	}
	return true;
    },
    stop_space : function() {
	if (!this._inited) return false;
	this._obj().Stop();
	return true;
    },
    load_contents : function(contents_data_uuid) {	
	if (!this._inited) return;
	var url = this._get_contents_url + contents_data_uuid;
	if (!this._obj().LoadContents(url)) {
	    alert("load failed");
	}
    },
    get_status_timer : function() {
	this._get_status();
    },
};
