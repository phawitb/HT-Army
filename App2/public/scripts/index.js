// convert epochtime to JavaScripte Date object
function epochToJsDate(epochTime){
  return new Date(epochTime*1000);
}

// convert time to human-readable format YYYY/MM/DD HH:MM:SS
function epochToDateTime(epochTime){
  var epochDate = new Date(epochToJsDate(epochTime));
  var dateTime = epochDate.getFullYear() + "/" +
    ("00" + (epochDate.getMonth() + 1)).slice(-2) + "/" +
    ("00" + epochDate.getDate()).slice(-2) + " " +
    ("00" + epochDate.getHours()).slice(-2) + ":" +
    ("00" + epochDate.getMinutes()).slice(-2) + ":" +
    ("00" + epochDate.getSeconds()).slice(-2);

  return dateTime;
}

// function to plot values on charts
function plotValues(chart, timestamp, value){
  console.log(timestamp);
  var x = epochToJsDate(timestamp).getTime() + 3600000*7;   //shiptime zone +7
  var y = Number (value);
  if(chart.series[0].data.length > 40) {
    chart.series[0].addPoint([x, y], true, true, true);
  } else {
    chart.series[0].addPoint([x, y], true, false, true);
  }
}

// DOM elements
const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector('#authentication-bar');
const deleteModalElement = document.getElementById('delete-modal');
const deleteDataFormElement = document.querySelector('#delete-data-form');
const tableContainerElement = document.querySelector('#table-container');
const chartsRangeInputElement = document.getElementById('charts-range');
const loadDataButtonElement = document.getElementById('load-data');
const cardsCheckboxElement = document.querySelector('input[name=cards-checkbox]');
const gaugesCheckboxElement = document.querySelector('input[name=gauges-checkbox]');
const chartsCheckboxElement = document.querySelector('input[name=charts-checkbox]');
const settingCheckboxElement = document.querySelector('input[name=setting-checkbox]');

// DOM elements for sensor readings
const cardsReadingsElement = document.querySelector("#cards-div");
const gaugesReadingsElement = document.querySelector("#gauges-div");
const chartsDivElement = document.querySelector('#charts-div');
const settingDivElement = document.querySelector('#setting-div');
const tempElement = document.getElementById("temp");
const humElement = document.getElementById("hum");
const presElement = document.getElementById("pres");
const updateElement = document.getElementById("lastUpdate")

var u = "";

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    //toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;
    // get user UID to get data from database
    var uid = user.uid;
    console.log(uid);
    u = uid;
    // Database paths (with user UID)
    var dbPath = 'UsersData/' + uid.toString() + '/historys';
    var chartPath = 'UsersData/' + uid.toString() + '/charts/range';
    // Database references
    var dbRef = firebase.database().ref(dbPath);
    var chartRef = firebase.database().ref(chartPath);

    //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=--=-=-=-=-=-=-=

    // Update database with new range (input field)========================================================
    chartsRangeInputElement.onchange = () =>{
      chartRef.set(chartsRangeInputElement.value);
      console.log(chartsRangeInputElement.value)
      chartRange = Number(chartsRangeInputElement.value)
 
      chartT.destroy();
      chartH.destroy();
      chartT = createTemperatureChart();
      chartH = createHumidityChart();
      
      dbRef.orderByKey().limitToLast(chartRange).get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        console.log(jsonData)
        k = Object.keys(jsonData)
        console.log(Object.keys(jsonData))
        const T=[];
        const H=[];
        const TS=[];
        const F=[];
        
        for (let i = 0; i < k.length; i++) {
          T[i] = jsonData[k[i]].temperature
          H[i] = jsonData[k[i]].humidity
          TS[i] = jsonData[k[i]].timestamp
          F[i] = jsonData[k[i]].flag

          plotValues(chartT, jsonData[k[i]].timestamp, jsonData[k[i]].temperature);
          plotValues(chartH, jsonData[k[i]].timestamp, jsonData[k[i]].humidity);

          var content = '';
          content += '<tr>';
          content += '<td>' + epochToDateTime(TS[i]) + '</td>';
          content += '<td>' + T[i] + '</td>';
          content += '<td>' + H[i] + '</td>';
          
          if(F[i]=="WHITE"){
              content += '<td style="color:' + "BLACK" +'">'+ F[i] + '</td>';
          }
          else{
              content += '<td style="color:' + F[i] +'">'+ F[i] + '</td>';
          }

          content += '</tr>';
          $('#tbody').prepend(content);
        }
      });

    };
    
    //cardsCheckbox========================================================================================
    cardsCheckboxElement.addEventListener('change', (e) =>{
      if (cardsCheckboxElement.checked) {
        cardsReadingsElement.style.display = 'block';
        
      }
      else{
        cardsReadingsElement.style.display = 'none';
      }
    });

    //-----------
    dbRef.orderByKey().limitToLast(1).get().then(snapshot =>{
      var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
      jsonData = jsonData[Object.keys(jsonData)[0]]
      var temperature = jsonData.temperature;
      var humidity = jsonData.humidity;
      var flag = jsonData.flag;
      var timestamp = jsonData.timestamp;
      // Update DOM elements
      tempElement.innerHTML = temperature;
      humElement.innerHTML = humidity;
      presElement.innerHTML = flag;
      updateElement.innerHTML = epochToDateTime(timestamp);

      if(flag=="WHITE"){
          presElement.style.color = "BLACK";
      }
      else{
          presElement.style.color = flag;
      }
    });
    //-----------

    //gaugesCheckbox=======================================================================================
    gaugesCheckboxElement.addEventListener('change', (e) =>{
      if (gaugesCheckboxElement.checked) {
        gaugesReadingsElement.style.display = 'block';
      }
      else{
        gaugesReadingsElement.style.display = 'none';
      }
    });
    //-------
    dbRef.orderByKey().limitToLast(1).get().then(snapshot =>{
      var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
      jsonData = jsonData[Object.keys(jsonData)[0]]
      var temperature = jsonData.temperature;
      var humidity = jsonData.humidity;
      var flag = jsonData.flag;
      var timestamp = jsonData.timestamp;
      // Update DOM elements
      var gaugeT = createTemperatureGauge();
      var gaugeH = createHumidityGauge();
      gaugeT.draw();
      gaugeH.draw();
      gaugeT.value = temperature;
      gaugeH.value = humidity;
      updateElement.innerHTML = epochToDateTime(timestamp);
    });
    //-------

    //chartsCheckbox=======================================================================================
    chartsCheckboxElement.addEventListener('change', (e) =>{
      console.log('chart........')
      if (chartsCheckboxElement.checked) {
        chartsDivElement.style.display = 'block';
        tableContainerElement.style.display = 'block';

        var chartRange = 0;
        chartRef.get().then(snapshot =>{

          console.log(chartRange);
          chartT.destroy();
          chartH.destroy();
          
          chartT = createTemperatureChart();
          chartH = createHumidityChart();
         
          chartRange = 5
          dbRef.orderByKey().limitToLast(chartRange).get().then(snapshot =>{
            var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
            console.log(jsonData)
            k = Object.keys(jsonData)
            console.log(Object.keys(jsonData))
            const T=[];
            const H=[];
            const TS=[];
            const F=[];
            
            for (let i = 0; i < k.length; i++) {
              T[i] = jsonData[k[i]].temperature
              H[i] = jsonData[k[i]].humidity
              TS[i] = jsonData[k[i]].timestamp
              F[i] = jsonData[k[i]].flag

              plotValues(chartT, jsonData[k[i]].timestamp, jsonData[k[i]].temperature);
              plotValues(chartH, jsonData[k[i]].timestamp, jsonData[k[i]].humidity);

              var content = '';
              content += '<tr>';
              content += '<td>' + epochToDateTime(TS[i]) + '</td>';
              content += '<td>' + T[i] + '</td>';
              content += '<td>' + H[i] + '</td>';
              
              if(F[i]=="WHITE"){
                  content += '<td style="color:' + "BLACK" +'">'+ F[i] + '</td>';
              }
              else{
                  content += '<td style="color:' + F[i] +'">'+ F[i] + '</td>';
              }

              content += '</tr>';
              $('#tbody').prepend(content);
            }
          });
        });
      }
      else{
        chartsDivElement.style.display = 'none';
      }
    });

    //settingCheckbox==========================================================================================
    settingCheckboxElement.addEventListener('change', (e) =>{

      if (settingCheckboxElement.checked) {
        settingDivElement.style.display = 'block';
        cardsReadingsElement.style.display = 'none';
        gaugesReadingsElement.style.display = 'none';
        chartsDivElement.style.display = 'none';
        tableContainerElement.style.display = 'none';

        chartsCheckboxElement.checked = false
        
        const line1 = document.getElementById('line1')
        const line2 = document.getElementById('line2')
        const line3 = document.getElementById('line3')
        const line4 = document.getElementById('line4')
        const line5 = document.getElementById('line5')
        const time2 = document.getElementById('time2')
        const time3 = document.getElementById('time3')
        const time4 = document.getElementById('time4')
        const time5 = document.getElementById('time5')
        const time6 = document.getElementById('time6')
        const time7 = document.getElementById('time7')
        const time8 = document.getElementById('time8')
        const gps0 = document.getElementById('gps0')
        const gps1 = document.getElementById('gps1')
        const adjusterror0 = document.getElementById('adjusterror0')
        const adjusterror1 = document.getElementById('adjusterror1')
        const unit = document.getElementById('unit')

        var i = 0;
        firebase.database().ref('UsersData/' + uid.toString() + '/config/line').orderByKey().on('child_added', snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          document.getElementById('line'+(i+1).toString()).value = jsonData;
          i+=1
        });
        var ii = 0;
        firebase.database().ref('UsersData/' + uid.toString() + '/config/time').orderByKey().on('child_added', snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          document.getElementById('time'+(ii+1).toString()).value = jsonData;
          ii+=1
        });
        firebase.database().ref('UsersData/' + uid.toString() + '/config/everyhour').orderByKey().on('child_added', snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          console.log(jsonData)
          document.getElementById('everyhour').checked = jsonData;
        });
        var iii = 0;
        firebase.database().ref('UsersData/' + uid.toString() + '/config/gps').orderByKey().on('child_added', snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          console.log(jsonData)
          document.getElementById('gps'+iii.toString()).value = jsonData;
          iii += 1;
        });
        var iiii = 0;
        firebase.database().ref('UsersData/' + uid.toString() + '/config/adjusterror').orderByKey().on('child_added', snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          console.log(jsonData)
          document.getElementById('adjusterror'+iiii.toString()).value = jsonData;
          iiii += 1;
        });
        firebase.database().ref('UsersData/' + uid.toString() + '/config/unit').orderByKey().on('child_added', snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          console.log(jsonData)
          document.getElementById('unit').value = jsonData;
          document.getElementById('authentication-status').innerHTML = jsonData;
        });
      }
      else{
        settingDivElement.style.display = 'none';
        cardsReadingsElement.style.display = 'block';
        gaugesReadingsElement.style.display = 'block';
      }
    });
  } else{
    // toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
}

function myFunction() {

  console.log(line1.value);
  console.log(time1.value);

  var checkedValue = document.getElementById('everyhour');
  console.log(checkedValue.checked);

  firebase.database().ref('UsersData/' + u.toString() + '/config/line').set({
      0 : line1.value,
      1 : line2.value,
      2 : line3.value,
      3 : line4.value,
    });

  firebase.database().ref('UsersData/' + u.toString() + '/config/time').set({
    0 : time1.value,
    1 : time2.value,
    2 : time3.value,
    3 : time4.value,
    4 : time5.value,
    5 : time6.value,
    6 : time7.value,
    7 : time8.value,
  });

  firebase.database().ref('UsersData/' + u.toString() + '/config/everyhour').set({
    status : checkedValue.checked,
  });
  firebase.database().ref('UsersData/' + u.toString() + '/config/gps').set({
    lat : gps0.value,
    lon : gps1.value
  });
  firebase.database().ref('UsersData/' + u.toString() + '/config/adjusterror').set({
    humid : adjusterror0.value,
    temp : adjusterror1.value
  });
  firebase.database().ref('UsersData/' + u.toString() + '/config/unit').set({
    name : unit.value,
  });

  window.alert("update complete!");

}

function myFunction2() {

  document.getElementById('everyhour').checked = false;
  document.getElementById('time1').value = "08:00";
  document.getElementById('time2').value = "10:00";
  document.getElementById('time3').value = "13:00";
  document.getElementById('time4').value = "15:00";
  document.getElementById('time5').value = "17:00";
  document.getElementById('time6').value = ":";
  document.getElementById('time7').value = ":";
  document.getElementById('time8').value = ":";

}

