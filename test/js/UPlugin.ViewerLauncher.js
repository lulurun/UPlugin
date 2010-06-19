var gViewerLauncher = gViewerLauncher || {
	_uplugin_name : 'ViewerLauncher',
	_uplugin : null,
	_obj : function() {
	if (this._uplugin == null) {
		return null;
	}
	return this._uplugin.Obj(); 
	},
	init : function() {
		if (this._uplugin == null) {
			this._uplugin = new UPlugin(this._uplugin_name, '');
			this._uplugin.CreateAppObj();
		}
	},
	destroy : function() {
		if (this._uplugin != null) {
			this._uplugin.DestoryAppObj();
			this._uplugin = null;
		}
	},
	getViewerList : function() {
		var res_json = this._obj().ViewerList();
		return eval('(' + res_json + ')');
	},
	launch : function(name, first, last, password, loginserver, location) {
		this._obj().Launch(name, first, last, password, loginserver, location);
	},
	close : function(name) {
		this._obj().Close(name);
	}
};

