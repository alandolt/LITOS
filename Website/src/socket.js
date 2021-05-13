import Vue from 'vue'

const host = 'ws://' + document.location.host + '/litosws'
// const host = 'ws://192.168.1.6/litosws' // For Local Testing via npm run serve

const socket = new WebSocket(host)

const emitter = new Vue({
  methods: {
    send(message) {
      if (socket.readyState === 1) { socket.send(message) }
    }
  }
})

socket.onopen = function() {
  emitter.$emit('connected')
}

socket.onclose = function() {
  emitter.$emit('disconnected')
}

socket.onmessage = function(msg) {
  emitter.$emit('message', JSON.parse(msg.data))
}
socket.onerror = function(err) {
  emitter.$emit('error', err)
}

setInterval(() => {
  if (socket.readyState === 0) {
    emitter.$emit('disconnected')
  } else if (socket.readyState === 3) {
    location.reload()
  } else {
    socket.send('{"command":"ping"}')
  }
}, 5000)

export default emitter

