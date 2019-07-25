/*
 *   This module has functions that facilitate communication with a web back-end
 *
 */

//ToDo: Write functions that use ajaxbusy
// used like this:
//	ajax({
//		operation: 'GET',
//		datafdat: {
//			id: 5,
//			newvalue: 'haha'
//		},
//		ajaxbusy: 'default'
//	})
//
// and that would use simplefullstack.ajaxbusy.default as the busy boolean

// allow "Keep-Alive" (create new ajaxrequest object using simplefullstack.createajax.
// request.setRequestHeader('Connection', 'Keep-Alive');
//


var simplefullstack = {
	ajaxbusy: {}
};

function ajax(params) {
	
	var request = new XMLHttpRequest();
	
	var operation = params.operation || 'POST';
	var async = (params.async === undefined) || params.async;
	var data = params.fdat ? converttoquerystring(params.fdat) : params.data;
	var resource = params.resource;
	var onfailure = params.onfailure;
	var onsuccess = params.onsuccess;
	
	request.onreadystatechange = function() {
	
		if (this.readyState == 4) {
			
			if (this.status == 200) {
				
				if (onsuccess) {
					
					onsuccess.call(this, this.responseText);
				}
			}
			else {
				
				if (onfailure) {
					
					// ToDo: use actual error text instead of just 'Error'
					onfailure.call(this, 'Error');
				}
			}
		}
	};
	
	request.open(operation, resource, async);

	request.send(data);
}

function converttoquerystring(fdat) {
	
	//ToDo make grep work properly with objects so that this can be done
	/*
	var entries = fdat.grep(function(key, val) {
		
		return val !== undefined;
	}).map(function(key, val) {
		
		return encodeURIComponent(key) + '=' + encodeURIComponent(val);
	});
*/
	var entries = [];
	
	fdat.for((key, val) => {
		
		if (val !== undefined) {
		
			entries.push(encodeURIComponent(key) + '=' + encodeURIComponent(val));
		}
	});
	
	return entries.join('&');
}

function parsequerystring(querystring) {
	
	var retrn = {};
	
	if (querystring) {
	
		var items = querystring.split('&');
		
		items.for(function(index, val) {
			
			var keyval = val.split('=');
//ToDo: make more robust in case of errors		
			var key = keyval[0];
			var value = keyval[1];
			retrn[decodeURIComponent(key)] = decodeURIComponent(value);
//alert('before: ' + key + '-------\n\n' + value);
//alert('after: ' + decodeURIComponent(key) + '-------\n\n' + decodeURIComponent(value));
		});
	}
	
	return retrn;
}
