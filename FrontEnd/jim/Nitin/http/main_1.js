
var handles = {};
var ajaxbusy;

function init() {

	document.body.appendChildren([
		'input', {
			id: 'txtdata',
			value: '(new data to send to server)'
		},
		'input', {
			type: 'button',
			value: 'Click Me',
			onclick: clicked
		},
		'div', {
			id: 'datadiv',
			innerHtml: 'No Data'
		}
	], handles);
	
	alert("Will get data every 5 seconds");
	
	setInterval(getdata, 5000);
}

function getdata() {
	
	if (!ajaxbusy) {
	
		ajaxbusy = true;
	
		ajax({
			resource: 'getdata',
			onsuccess: (response) => {
				handles.datadiv.innerHTML = "Got data: " + response;
				ajaxbusy = false;
			},
			onfailure: (error) => {
				ajaxbusy = false;
			}
		});
	}
}

function clicked() {
	
	ajax({
		resource: 'buttonclicked',
		fdat: {
			data: handles.txtdata.value
		},
		onsuccess: () => {
			alert("Button click event sent to server");
		},
		onfailure: () => {
			alert("error reaching server");
		}
	});
}


