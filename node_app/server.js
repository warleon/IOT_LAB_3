// node_app/server.js
const express = require("express");
const mqtt = require("mqtt");

const app = express();
const port = 3000;

const messages = [];
// MQTT client setup
const mqttClient = mqtt.connect("mqtt://mosquitto_broker:1883");

mqttClient.on("error", (error) => {
  console.error("MQTT connection error:", error);
  mqttClient.reconnect(); // Attempt to reconnect on error
});

mqttClient.subscribe("test/topic", (err) => {
  if (err) {
    console.log("Subscription error:", err);
  }
});
mqttClient.on("message", (topic, message) => {
  console.log(`Received message on ${topic}: ${message.toString()}`);
  messages.push(message.toString());
});

// API endpoint to retrieve the latest MQTT message
app.get("/api/message", (req, res) => {
  res.json({ topic: "test/topic", messages: messages });
});

app.listen(port, () => {
  console.log(`Express app listening on http://localhost:${port}`);
});
