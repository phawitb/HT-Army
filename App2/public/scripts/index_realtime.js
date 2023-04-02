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
    // x += 3600000;
    // console.log(x);
    // console.log(x+3600000);
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
//   const deleteButtonElement = document.getElementById('delete-button');
  const deleteModalElement = document.getElementById('delete-modal');
  const deleteDataFormElement = document.querySelector('#delete-data-form');
  const viewDataButtonElement = document.getElementById('view-data-button');
  const hideDataButtonElement = document.getElementById('hide-data-button');
  const tableContainerElement = document.querySelector('#table-container');
  const chartsRangeInputElement = document.getElementById('charts-range');
  const loadDataButtonElement = document.getElementById('load-data');
  const cardsCheckboxElement = document.querySelector('input[name=cards-checkbox]');
  const gaugesCheckboxElement = document.querySelector('input[name=gauges-checkbox]');
  const chartsCheckboxElement = document.querySelector('input[name=charts-checkbox]');
  
  // DOM elements for sensor readings
  const cardsReadingsElement = document.querySelector("#cards-div");
  const gaugesReadingsElement = document.querySelector("#gauges-div");
  const chartsDivElement = document.querySelector('#charts-div');
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

      //=========
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
      firebase.database().ref('UsersData/' + uid.toString() + '/config/line').orderByKey().get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        document.getElementById('line'+(i+1).toString()).value = jsonData;
        i+=1
  
      });

      var ii = 0;
      firebase.database().ref('UsersData/' + uid.toString() + '/config/time').orderByKey().get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        document.getElementById('time'+(ii+1).toString()).value = jsonData;
        ii+=1
  
      });

      firebase.database().ref('UsersData/' + uid.toString() + '/config/everyhour').orderByKey().get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        console.log(jsonData)
        document.getElementById('everyhour').checked = jsonData;
  
      });

      var iii = 0;
      firebase.database().ref('UsersData/' + uid.toString() + '/config/gps').orderByKey().get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        console.log(jsonData)
        document.getElementById('gps'+iii.toString()).value = jsonData;
        iii += 1;
  
      });

      var iiii = 0;
      firebase.database().ref('UsersData/' + uid.toString() + '/config/adjusterror').orderByKey().get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        console.log(jsonData)
        document.getElementById('adjusterror'+iiii.toString()).value = jsonData;
        iiii += 1;
  
      });

      firebase.database().ref('UsersData/' + uid.toString() + '/config/unit').orderByKey().get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        console.log(jsonData)
        document.getElementById('unit').value = jsonData;
        document.getElementById('authentication-status').innerHTML = jsonData;
        
      });


      // CHARTS
      // Number of readings to plot on charts
      var chartRange = 0;
      // Get number of readings to plot saved on database (runs when the page first loads and whenever there's a change in the database)
      chartRef.get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
        jsonData = jsonData[Object.keys(jsonData)[0]]
        chartRange = Number(jsonData);
        if(chartRange > 20){
          chartRange = 20;
        }
        console.log(chartRange);
        // Delete all data from charts to update with new values when a new range is selected
        chartT.destroy();
        chartH.destroy();
        // chartP.destroy();
        // Render new charts to display new range of data
        chartT = createTemperatureChart();
        chartH = createHumidityChart();
        // chartP = createFlagChart();
        // Update the charts with the new range
        // Get the latest readings and plot them on charts (the number of plotted readings corresponds to the chartRange value)
        dbRef.orderByKey().limitToLast(chartRange).get().then(snapshot =>{
          var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
          jsonData = jsonData[Object.keys(jsonData)[0]]
          // Save values on variables
          var temperature = jsonData.temperature;
          var humidity = jsonData.humidity;
          var flag = jsonData.flag;
          var timestamp = jsonData.timestamp;
          // Plot the values on the charts
          plotValues(chartT, timestamp, temperature);
          plotValues(chartH, timestamp, humidity);
        //   plotValues(chartP, timestamp, flag);
        });
      });
  
      // Update database with new range (input field)
      chartsRangeInputElement.onchange = () =>{
        chartRef.set(chartsRangeInputElement.value);
      };
  
      //CHECKBOXES
      // Checbox (cards for sensor readings)
      cardsCheckboxElement.addEventListener('change', (e) =>{
        if (cardsCheckboxElement.checked) {
          cardsReadingsElement.style.display = 'block';
        }
        else{
          cardsReadingsElement.style.display = 'none';
        }
      });
      // Checbox (gauges for sensor readings)
      gaugesCheckboxElement.addEventListener('change', (e) =>{
        if (gaugesCheckboxElement.checked) {
          gaugesReadingsElement.style.display = 'block';
        }
        else{
          gaugesReadingsElement.style.display = 'none';
        }
      });
      // Checbox (charta for sensor readings)
      chartsCheckboxElement.addEventListener('change', (e) =>{
        if (chartsCheckboxElement.checked) {
          chartsDivElement.style.display = 'block';
        }
        else{
          chartsDivElement.style.display = 'none';
        }
      });
  
      // CARDS
      // Get the latest readings and display on cards
      dbRef.orderByKey().limitToLast(1).get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
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
  
      // GAUGES
      // Get the latest readings and display on gauges
      dbRef.orderByKey().limitToLast(1).get().then(snapshot =>{
        var jsonData = snapshot.toJSON(); // example: {temperature: 25.02, humidity: 50.20, flag: 1008.48, timestamp:1641317355}
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
  
      // DELETE DATA
      // Add event listener to open modal when click on "Delete Data" button
    //   deleteButtonElement.addEventListener('click', e =>{
    //     console.log("Remove data");
    //     e.preventDefault;
    //     deleteModalElement.style.display="block";
    //   });
  
      // Add event listener when delete form is submited
      deleteDataFormElement.addEventListener('submit', (e) => {
        // delete data (readings)
        dbRef.remove();
      });
  
      // TABLE
      var lastReadingTimestamp; //saves last timestamp displayed on the table
      // Function that creates the table with the first 100 readings
      function createTable(){
        // append all data to the table
        var firstRun = true;
        dbRef.orderByKey().limitToLast(100).get().then(function(snapshot) {
          if (snapshot.exists()) {
            var jsonData = snapshot.toJSON();
            jsonData = jsonData[Object.keys(jsonData)[0]]
            console.log(jsonData);
            var temperature = jsonData.temperature;
            var humidity = jsonData.humidity;
            var flag = jsonData.flag;
            var timestamp = jsonData.timestamp;
            var content = '';
            content += '<tr>';
            content += '<td>' + epochToDateTime(timestamp) + '</td>';
            content += '<td>' + temperature + '</td>';
            content += '<td>' + humidity + '</td>';
            // content += '<td>' + flag + '</td>';
            if(flag=="WHITE"){
                content += '<td style="color:' + "BLACK" +'">'+ flag + '</td>';
            }
            else{
                content += '<td style="color:' + flag +'">'+ flag + '</td>';
            }
            
            // <td style="color:red">$100</td>
            content += '</tr>';
            $('#tbody').prepend(content);
            // Save lastReadingTimestamp --> corresponds to the first timestamp on the returned snapshot data
            if (firstRun){
              lastReadingTimestamp = timestamp;
              firstRun=false;
              console.log(lastReadingTimestamp);
            }
          }
        });
      };
  
      // append readings to table (after pressing More results... button)
      function appendToTable(){
        var dataList = []; // saves list of readings returned by the snapshot (oldest-->newest)
        var reversedList = []; // the same as previous, but reversed (newest--> oldest)
        console.log("APEND");
        dbRef.orderByKey().limitToLast(100).endAt(lastReadingTimestamp).get().then(function(snapshot) {
          // convert the snapshot to JSON
          if (snapshot.exists()) {
            snapshot.forEach(element => {
              var jsonData = element.toJSON();
              jsonData = jsonData[Object.keys(jsonData)[0]]
              dataList.push(jsonData); // create a list with all data
            });
            lastReadingTimestamp = dataList[0].timestamp; //oldest timestamp corresponds to the first on the list (oldest --> newest)
            reversedList = dataList.reverse(); // reverse the order of the list (newest data --> oldest data)
  
            var firstTime = true;
            // loop through all elements of the list and append to table (newest elements first)
            reversedList.forEach(element =>{
              if (firstTime){ // ignore first reading (it's already on the table from the previous query)
                firstTime = false;
              }
              else{
                var temperature = element.temperature;
                var humidity = element.humidity;
                var flag = element.flag;
                var timestamp = element.timestamp;
                var content = '';
                content += '<tr>';
                content += '<td>' + epochToDateTime(timestamp) + '</td>';
                content += '<td>' + temperature + '</td>';
                content += '<td>' + humidity + '</td>';
                content += '<td>' + flag + '</td>';
                content += '</tr>';
                $('#tbody').append(content);
              }
            });
          }
        });
      }
  
      viewDataButtonElement.addEventListener('click', (e) =>{
        // Toggle DOM elements
        tableContainerElement.style.display = 'block';
        viewDataButtonElement.style.display ='none';
        hideDataButtonElement.style.display ='inline-block';
        loadDataButtonElement.style.display = 'inline-block'
        createTable();
      });
  
      loadDataButtonElement.addEventListener('click', (e) => {
        appendToTable();
      });
  
      hideDataButtonElement.addEventListener('click', (e) => {
        tableContainerElement.style.display = 'none';
        viewDataButtonElement.style.display = 'inline-block';
        hideDataButtonElement.style.display = 'none';
      });
  
    // IF USER IS LOGGED OUT

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

