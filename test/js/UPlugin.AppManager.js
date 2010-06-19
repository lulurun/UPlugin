var gAppManager = gAppManager || {
    _uplugin_name : 'AppManager',
    _uplugin : null,
    _obj : function() {
	if (this._uplugin == null) {
	    return null;
	}
	return this._uplugin.Obj(); 
    },
    STATE : {
	ERR_VERSION_NOT_FOUND : -100,
	ERROR_DUP : -2,
	ERROR : -1,
	CHK_VERSION : 1,
	DOWNLOAD_PKG : 2,
	EXTRACT_PKG : 3,
	INSTALL : 4,
	FINISH : 5,
	CHK_UPDATE : 100,
	DEFAULT : 0
    },
    MESSAGE : function(state) {
	switch(state) {
	case this.STATE.ERR_VERSION_NOT_FOUND:
	  return "Can not find specified version";
	case this.STATE.ERROR_DUP:
	  return "Already in Process";
	case this.STATE.ERROR:
	  return "ERROR";
	//case this.STATE.DEFAULT:
	//  return "";
	case this.STATE.CHK_VERSION:
	  return "Checking Current Version";
	case this.STATE.DOWNLOAD_PKG:
	  return "Downloading";
	case this.STATE.EXTRACT_PKG:
	  return "Extracting";
	case this.STATE.INSTALL:
	  return "Installing";
	case this.STATE.FINISH:
	  return "Finish";
	case this.STATE.CHK_UPDATE:
	  return "Checking Update";
	}
	return ". . . ";
    },
    // Constants
    Status : 0,
    _wait_install_timer : null,
    _OnStateChange : null,

    init : function() {
	if (this._uplugin == null) {
	    this._uplugin = new UPlugin(this._uplugin_name, '');
	    this._uplugin.CreateAppObj();
	}
	_OnStateChange = null;
    },
    destroy : function() {
	if (this._uplugin != null) {
	    this._uplugin.DestoryAppObj();
	    this._uplugin = null;
	}
	this.Status = 0;
	this._wait_install_timer = null;
	this._OnStateChange = null;
    },
    install : function(name, vi_url, key_file, version, OnStateChange) {
	this._OnStateChange = OnStateChange;
	if (this._OnStateChange != null) this._OnStateChange.exec(this.STATE.CHK_VERSION);	
	// check current version
	var installed = this.getInstalled(name);
	var should_install = false;
	if (installed != null) {
	    if (installed.version != version) {
		this.uninstall(name, OnStateChange);
		should_install = true;
	    }
	} else {
	    should_install = true;
	}
	if (!should_install) {
	    if (this._OnStateChange != null) this._OnStateChange.exec(this.STATE.FINISH);
	}
	// get version info
	if (this._OnStateChange != null) this._OnStateChange.exec(this.STATE.CHK_UPDATE);
	
	$.getJSON(vi_url + "&callback=?", function(data){
		var list = data["list"];
		var url = "";
		if (!version) {
		    version = data["latest"];
		}
		for(var i in list){
		    if (list[i]['ver'] == version) {
			url = list[i]['url'];
		    }
		}		
		if (!url) {
		    if (gAppManager._OnStateChange != null) gAppManager._OnStateChange.exec(gAppManager.STATE.ERR_VERSION_NOT_FOUND);
		    return;
		}
		// start install
		gAppManager._obj().Install(name, url, key_file, version);
		_wait_install_timer = setInterval("gAppManager.wait_install_timer()" ,500);    
	    });
    },
    wait_install_timer : function() {
	var status = gAppManager._obj().GetStatus();	
	if (this.Status != status && this._OnStateChange != null) {
	    this._OnStateChange.exec(status);
	}
	this.Status = status;
	if (this.Status == this.STATE.FINISH || this.Status == this.STATE.ERROR || this.Status == this.STATE.DEFAULT) {
	    // job finish
	    clearInterval(_wait_install_timer);
	    this._OnStateChange = null;
	}
    },
    uninstall : function(name, OnStateChange) {
	this._OnStateChange = OnStateChange;
	// TODO @@@ async !!
	var res = this._obj().Uninstall(name);
	if (this._OnStateChange != null) {
	    this._OnStateChange.exec(res ? this.STATE.FINISH : this.STATE.ERROR);
	}
	return res;
    },
    isInstalled : function(name) {
	var installed = this.getInstalled(name);
	return installed == null ? false : true;
    },
    getInstalled : function(name) {
	var installed = null;
	var list = this.getAppList();
	for(var i in list) {
	    if (list[i].name == name) {
		installed = list[i];
		break;
	    }
	}
	return installed;
    },
    getAppList : function() {
	var res_json = this._obj().GetAppList();
	return eval('(' + res_json + ')');
    }
};

