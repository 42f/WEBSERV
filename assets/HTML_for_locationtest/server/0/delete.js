

function sendDelete () {
  var oReq = new XMLHttpRequest();
  oReq.open("DELETE", "http://localhost:18000/site0/anotherfile.html");


  oReq.send();
}
