gyroTopic.subscribe((value) => {
  console.log(`Got Gyro Value: ${value}`);
});

subscribe((value) => {
  console.log(`Got Gyro Value: ${value}`);
}, {}, getNextSubuid, true);

messenger.subscribe({
topics: /MyTable/Gyro, 
subuid,
options ({})
});

sendTextFrame({
  method: 'subscribe',
  {
topics: /MyTable/Gyro, 
subuid,
options ({})
})

websocket.send(JSON.stringify([
  {
  method: 'subscribe',
  {
topics: /MyTable/Gyro, 
subuid,
options ({})
}
])
