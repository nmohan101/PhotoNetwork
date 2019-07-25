/*
 *   This module has functions for performing DOM operations
 *
 */


/*
 *  create
 *	Wrapper function for creating DOM elements with document.createElement
 */
function create(htmltag, objectdef, idtracker) {

	if (isarray(objectdef)) {
		
		// Array Mode
		
		return objectdef.map(function() {

			return create(htmltag, this, idtracker);
		});
	}
	
	// Single Mode
	
	var isnselement = [
		'svg',
		'rect'
	].indexOf(htmltag) > -1;
	
	var newobject;

	if (htmltag == 'text') {

		newobject = document.createTextNode(objectdef);
	}
	else if (htmltag == 'option') {

		newobject = new Option(
			objectdef.text || objectdef.name,
			objectdef.value || objectdef.id
		);
	}
	else if (htmltag == 'simpletable') {
		
		var newobjectdef = {
			children: []
		};
		
		function parserowsandcells(rows) {
		
			for (var rowsindex = 0; rowsindex < rows.length; rowsindex++) { 
					
				var currentitem = rows[rowsindex];
				var rowdef = {
					children: []
				};
				var celllist;
				
				if (isarray(currentitem)) {
				
					celllist = currentitem;
				}
				else if (isobject(this)) {
					
					// Important to append rather than set equal so that we don't modify the
					// input parameter object by adding a 'children' member
					rowdef.append(currentitem);
					
					celllist = rows[++rowsindex]
				}
				else {
					
					throw('tbody, thead, tfoot members of simpletable must contain only row definition objects and arrays of cells');
				}
			
				if (!isarray(celllist)) {
					
					throw('tbody, thead, tfoot members of simpletable must not contain two row definition objects in a row. They need to alternate with cell arrays')
				}
				
				rowdef.children = celllist.map(function() {
					
					return create('td', this, idtracker);
				});
				
				newobjectdef.children.push(create('tr', rowdef, idtracker));
			}
		}
		
		objectdef.for(function(memberkey, memberval) {

			if (memberkey == 'tbody') {
				
				if (!isarray(memberval)) {
					
					throw('tbody member of simpletable must be an array');
				}
			
				parserowsandcells(memberval);
			}
			else {
				newobjectdef[memberkey] = memberval;
			}
		});
		
		newobject = create('table', newobjectdef, idtracker);
		
	}
	else if (isnselement) {
	
		newobject = document.createElementNS('http://www.w3.org/2000/svg', htmltag);
	}
	else {

		newobject = document.createElement(htmltag);
	}

	if (isobject(objectdef)) {

		for (var key in objectdef) {

			var value = objectdef[key];

			if (key == 'style') {

				var styleobject = value;

				styleobject.for(function (stylekey) {

					newobject.style[stylekey] = this;
				});
			}
			else if (key == 'appendChildren' || key == 'children') {

				newobject.appendChildren(value, idtracker);
			}
			else if (key == 'options') {

				var domobjects = create('option', value, idtracker);

				domobjects.for(function() {

					newobject.add(this);
				});
			}
			else {
				if (isnselement) {
				
					newobject.setAttribute(key, value);
				}
				else {
				
					newobject[key] = value;
				}
			}
		}

		if (idtracker && objectdef.id) {

			idtracker[objectdef.id] = newobject;
		}
	}
	
	return newobject;
}

/*
 *  appendChildren
 *	Same as the built in Node.appendChild() function, except it
 *	takes an array of multiple children to append. If the next child is not a dom object, create()
 *	will be called on the next 2 parameters. This avoids having lists like "create(), create(), create()"
 */
Object.defineProperty(Object.prototype, 'appendChildren', {
	value: function(children, idtracker) {
			
		for (var a = 0; a < children.length; a++) {

			var child = children[a];

			if (isobject(child)) {

				// Array element is an object. Treat it as a DOM Node.

				this.appendChild(child);
				
				if (child.id && idtracker) {
					
					idtracker[child.id] = child;
				}
			}
			else if (isarray(child)) {

				// Array element is an array. Pass it recursively to process it.

				this.appendChildren(child, idtracker);
			}
			else {
				// Array element is an html tag. Process it along with the following Array element and advance the index accordingly
				
				var createresult = create(child, children[++a], idtracker);
				
				if (!isarray(createresult)) {
					
					// appendChildren expects an array, but create can return a single DOM Node in some cases
					
					createresult = [createresult];
				}
				
				this.appendChildren(createresult, idtracker);
			}
		}
	}
});

/*
 *  getpageoffsets
 *	gets the absolute position of the calling DOM object
 */
Object.defineProperty(Object.prototype, 'getpageoffsets', {
	value: function getpageoffsets() {
	
		var offsetnode = this;

		var totaloffset = {
			left: 0,
			top: 0
		};

		while (offsetnode && offsetnode.offsetTop !== undefined) {

			totaloffset.left += offsetnode.offsetLeft;
			totaloffset.top += offsetnode.offsetTop;
			offsetnode = offsetnode.parentNode;
		}

		totaloffset.left = totaloffset.left;
		totaloffset.top = totaloffset.top;

		return totaloffset;
	}
});