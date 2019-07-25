/*
 *   This module has functions that greatly increase the efficiency and elegance
 *	of javascript code with respect to DOM operations and data-driven functional programming
 *
 */


//ToDo: need "params' function that checks parameters. make it congruent with the c++ version

/*
 *  getbyid
 *	Alias for document.getElementById, which is too verbose for how often it is used
 */
function getbyid(id) {
	
	return document.getElementById(id);
}

/*
 *  isarray
 *	return true is val is an array
 */
function isarray(val) {
	
	return val ? val.constructor === Array : false;
}

/*
 *  isobject
 *	returns true if val is an object
 */
function isobject(val) {

	return (
		!isarray(val)
		&& val === Object(val)
	);
}

/*
 *  gatherarguments
 *	Takes a list of arguments and converts it into a proper array
 */
function gatherarguments(theargs) {
	
	var properarray = [];
				
	for (var a = 0; a < theargs[0].length; a++) {

		properarray[a] = theargs[0][a];
	}
	
	return properarray;
}

/*
 *  Array.for
 *  Object.for
 *	Similar to perls for function.
 */
Object.defineProperty(Array.prototype, 'for', {
	value: function(loopfunction) {

		for (var a = 0; a < this.length; a++) {

			loopfunction.call(this[a], a, this[a]);
		}
	}
});
Object.defineProperty(Object.prototype, 'for', {
	value: function(loopfunction) {

		for (var key in this) {

			loopfunction.call(this[key], key, this[key]);
		}
	}
});

/*
 *  Array.map
 *  Object.map
 *	Similar to perl's map function.
 */
Object.defineProperty(Array.prototype, 'map', {
	value: function(loopfunction) {	
	
		var returnelements = [];

		this.for(function(key) {
			returnelements.push(loopfunction.call(this, key, this));
		});

		return returnelements;
	}
});
Object.defineProperty(Object.prototype, 'map', {
	value: Array.prototype.map
});

/*
 *  Array.grep
 *  Object.grep
 *	Similar to perl's grep function.
 */
Object.defineProperty(Array.prototype, 'grep', {
	value: function(loopfunction) {

		var returnelements = [];

		this.for(function(key) {

			if (loopfunction.call(this, key, this)) {

				returnelements.push(this);
			}
		});

		return returnelements;
	}
});
Object.defineProperty(Object.prototype, 'grep', {
	value: Array.prototype.grep
});

/*
 *  Object.append
 *	Copies and adds all the members of another object
 */
Object.defineProperty(Object.prototype, 'append', {
	value: function(sourceobject) {
		
		var me = this;
		
		sourceobject.for(function(key, val) {
			
			me[key] = val;
		});
		
		return this;
	}
});

/*
 * Object.setstyle
 *	Sets the styles of the members of the calling object (which are DOM handles)
 *
 * Return: (object)
 *	Returns this
 *
 * Example:
 *	handles.setstyle({
 *		loginlink: {
 *			display: 'none',
 *			color: 'blue'
 *		},
 *		logindiv: {
 *			display: 'inline',
 *			color: 'red'
 *		}
 *	});
 */
Object.defineProperty(Object.prototype, 'setstyle', {
	value: function(stylebynamebyhandlekey) {

		for (var handlekey in stylebynamebyhandlekey) {

			var stylebyname = stylebynamebyhandlekey[handlekey];
			
			for (var name in stylebyname) {
			
				this[handlekey].style[name] = stylebyname[name];
			}
		}
		
		return this;
	}
});

/*
 * Object.setattr
 *	Sets the attributes of the members of the calling object (which are DOM handles)
 *
 * Return: (object)
 *	returns this
 *
 * Exmaple
 *	handles.setattr({
 *		userlink: {
 *			innerHTML: 'I am invisible',
 *			style: {
 * 				display: 'none',
 * 				color: 'blue'
 * 			}
 *		},
 *		otherlink: {
 *			innerHTML: 'I am visible',
 *			style: {
 * 				display: 'inline',
 * 				color: 'red'
 * 			}
 *		}
 *	});
 */
Object.defineProperty(Object.prototype, 'setattr', {
	value: function(attributebynamebyhandlekey) {

		for (var handlekey in attributebynamebyhandlekey) {

			var attributebyname = attributebynamebyhandlekey[handlekey];
			
			for (var name in attributebyname) {
				
				var attribute = attributebyname[name];
				
				if (name == 'style') {
					
					//ToDo: Find better way to do this. create object using 2 item array
					//	eg: createobject([key, attribute]);
					var stylebynamebyhandlekey = {};
					stylebynamebyhandlekey[handlekey] = attribute;

					this.setstyle(stylebynamebyhandlekey);
				}
				else {
				
					this[handlekey][name] = attribute;
				}
			}
		}
		
		return this;
	}
});

/*
 * Function.bindpreserve
 *	Same thing as bind, except it preserves "this" reference of the caller.
 *	Useful for bindind an event function with additional args
 *
 * Return: (function)
 *	returns a function that calls another function with arguments
 *
 * Example: Hitting enter on the textbox will generate message "Hello Jim!".
 *		The event will be passed from the input to the function.
 *
 *	function mykeydown(phrase, event) {
 *		
 *		if (event.keyCode == 13) {
 *			
 *			alert("phrase + ' ' + this.value + '!');
 *		}
 *	}
 *	create('input', {
 *		value: 'Jim',
 *		onkeydown: mykeydown.bindpreserve('Hello'),
 *	});
 */
Object.defineProperty(Function.prototype, 'bindpreserve', {
	value: function() {
		
		var me = this;
		var meargs = Array.prototype.slice.call(arguments);
		
		return function() {
			
			var args = Array.prototype.slice.call(arguments);
			me.apply(this, meargs.concat(args));
		}
	}
});

/*
 *  eq
 *	Returns true if the two passed objects have equivalent elements
 */
function eq(object1, object2, debug) {
	
	if (isarray(object1) || isobject(object1)) {
		
		if (
			isarray(object1) != isarray(object2)
			|| isobject(object1) != isobject(object2)
		) {
			if (debug) {
				
				alert('type:\n' + object1 + '\n' + object2);
			}
			
			return false;
		}
		
		var diffcount = object1.grep(function(key) {
				
			if (!eq(this, object2[key], debug)) {

				if (debug) {

					alert('object member: ' + key + '\n' + this + '\n' + object2[key]);
				}

				return true
			}

			return false;
		}).length;
		
		if (diffcount) {
			
			return false;
		}
	}
	else if (object1 != object2) {
		
		if (debug) {
								
			alert('value:\n' + object1 + '\n' + object2);
		}
		return false;
	}
	
	return true;
}