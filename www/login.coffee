login_error = 0

window.lw_login_form = ->
	login_form=document.createElement("form")
	login_form.action="
		javascript:window.ajax((document.getElementById('email').value+
		'@##@'+document.getElementById('password').value)),window.ajax('')"
	login_form.innerHTML = ""

	if(login_error > 0)
		login_form.innerHTML += "<strong>Error:</strong> Invalid user/password combination!<br><br>"
	
	# If we have to login more than 1 time
	login_error = 1

	login_form.innerHTML += "
		<strong>Email:</strong><br><input id=email type=email></input><p>
		<strong>Password:</strong><br><input id=password type=password></input><p>
		<input type=submit value=Login></input>"
	
	window.EvalScriptElem.parentNode.insertBefore login_form, window.EvalScriptElem


window.lw_logout_button = ->
	# Reset login errors on logout
	login_error = 0
	
	logout=document.createElement("a")

	logout.setAttribute 'onclick', 
		"javascript:window.setCookie('login', ''),window.ajax('')"

	logout.setAttribute 'id', 'logout_button'

	logout.setAttribute 'style', 'background:#FF6300;color:#FFF;cursor:pointer;
		margin-right:5px;width:9%;vertical-align: top;'

	logout.innerHTML = "<div style='margin:1px 10px;display: inline-block;'>
		Logout</div>"

	window.EvalScriptElem.parentNode.insertBefore logout, window.EvalScriptElem
	
