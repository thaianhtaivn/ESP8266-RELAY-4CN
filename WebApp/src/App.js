import "./App.scss";
import { Client, Message } from "paho-mqtt";
import { useState, useEffect } from "react";
function App() {
  const [client, setclient] = useState();
  const [relayData, setrelayData] = useState([
    { name: "RELAY 1", state: false },
    { name: "RELAY 2", state: false },
    { name: "RELAY 3", state: false },
    { name: "RELAY 4", state: false },
  ]);
  const [relayID, setrelayID] = useState("relay-demo");
  const options = {
    host: "42c1410bece7497a99cfa15427041510.s2.eu.hivemq.cloud",
    port: 8884,
    protocol: "mqtts",
    username: "dien-tu-tieu-hoc",
    password: "1234567890",
    clientId: `client-id-${relayID}`,
  };
  useEffect(() => {
    const client = new Client(options.host, options.port, options.clientId);
    setclient(client);
    function connect() {
      // set callback handlers
      client.onConnectionLost = onConnectionLost;
      client.onMessageArrived = onMessageArrived;

      // set options
      const connectOptions = {
        onSuccess: onConnect,
        useSSL: true,
        userName: options.username,
        password: options.password,
      };
      client.connect(connectOptions);
    }

    function onConnect() {
      console.log("Connected to HiveMQ broker");
      client.subscribe(`${relayID}`);
    }

    function onConnectionLost(responseObject) {
      if (responseObject.errorCode !== 0) {
        console.log("Connection lost:", responseObject.errorMessage);
      }
    }

    function onMessageArrived(message) {
      console.log("Received message:", message.payloadString);
    }

    connect();
    return () => {
      client.disconnect();
    };
  }, []);

  function handleSwitch(value) {
    const message = new Message(JSON.stringify(value));
    message.destinationName = relayID;
    message.qos = 0;
    console.log("Publish data: " + JSON.stringify(value) + " to " + message.destinationName);
    client.send(message);
  }

  return (
    <div className="App">
      <div className="app-header">
        <h3>RELAY CONTROLLER APP</h3>
      </div>
      <div className="switch-btns">
        <div className="relay-id d-flex">
          <label className="col-form-label me-2">Relay ID:</label>
          <div className="col">
            <input type="email" className="form-control" placeholder="Relay ID" onChange={(e) => setrelayID(e.target.value)} />
          </div>
        </div>
        {Object.values(relayData).map((item, index) => (
          <div className="form-check form-switch ps-0 mb-2" key={index}>
            <label className="">{item.name}</label>
            <input className="form-check-input" type="checkbox" defaultChecked={item.state} onChange={() => handleSwitch(item)} />
          </div>
        ))}
      </div>
    </div>
  );
}

export default App;
