
const ARDUINO_IP = "http://172.20.10.13/"; 
const dataEndpoint = ARDUINO_IP;        


// CHART
const ctx = document.getElementById("vitalChart").getContext("2d");

//the Chart has 2 yaxis
const vitalChart = new Chart(ctx, {
  type: "line",
  data: {
    labels: [],
    datasets: [
      { label: "Heart Rate (BPM)", data: [], yAxisID: "y1" },
      { label: "Acceleration (g)", data: [], yAxisID: "y2" },
    ],
  },
  options: {
    scales: {
      y1: { beginAtZero: true, position: "left" }, //Heart rate column is left
      y2: { beginAtZero: true, suggestedMax: 4, position: "right" },
    },
  },
});


// MAP
const map = L.map("map").setView([0, 0], 2);
L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png").addTo(map);
let marker = L.marker([0, 0]).addTo(map);

//function check if they can receive signal then return lat and lng
function updateMap(lat, lng) {
  if (!isFinite(lat)) return;
  marker.setLatLng([lat, lng]);
  map.setView([lat, lng], 16);
}


function updateCards(data) {
  document.getElementById("heartRate").textContent = data.bpm + " BPM";
  document.getElementById("heartStatus").textContent = data.heartRateStatus;

  // SỬA freefall → fall
  document.getElementById("fallStatusText").textContent =
    data.fall ? "FALL DETECTED" : "Normal";

  document.getElementById("gpsCoords").textContent = `${data.lat}, ${data.lng}`;
}

// FALL HISTORY
function addFallHistoryRow(data) {
  if (!data.fall) return;   // 🔥 FIXED: was freefall

  const tbody = document.getElementById("fallTableBody");
  const tr = document.createElement("tr");

  tr.innerHTML = `
    <td>${new Date().toLocaleString()}</td>
    <td>${data.bpm} BPM</td>
    <td>${data.lat}, ${data.lng}</td>
    <td style="color:red;">Fall</td>
  `;

  tbody.prepend(tr);
}

// FETCH DATA FROM ARDUINO
async function fetchData() {
  try {
    const res = await fetch(dataEndpoint);
    const data = await res.json();

    //call functions to update data
    updateCards(data);
    updateMap(data.lat, data.lng);
    addFallHistoryRow(data);


    vitalChart.data.labels.push(new Date().toLocaleTimeString());
    vitalChart.data.datasets[0].data.push(data.bpm);
    vitalChart.data.datasets[1].data.push(data.acc);
    vitalChart.update();


    document.getElementById("connectionStatus").textContent = "Online";
  } catch (error) {
    document.getElementById("connectionStatus").textContent = "Offline";
  }
}



setInterval(fetchData, 1500);
fetchData();



