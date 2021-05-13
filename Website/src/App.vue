<template>
  <div id="app">
    <router-view />
  </div>
</template>

<script>
import Socket from './socket'

export default {
  name: 'App',
  data() {
    return {
      ws: {
        url: '',
        connected: false
      }
    }
  },

  mounted() {
    Socket.$on('connected', () => {
      this.ws.connected = true
      Socket.send(JSON.stringify({ command: 'get_file_list' }))
    })
    setTimeout(function() {
      Socket.send(JSON.stringify({ command: 'get_settings' }))
    }, 500)

    Socket.$on('disconnected', () => {
      this.ws.connected = false
    })

    Socket.$on('message', json => {
      this.ws.connected = true
      if (json.response === 'get_file_list') {
        this.$store.commit('delete_file_list')
        json.files.forEach(json_file => {
          this.$store.commit('add_file', {
            file: json_file,
            filename: json_file.substring(6, json_file.length - 4)
          })
        })
      } else if (json.response === 'get_settings') {
        this.$store.commit('save_settings', {
          obj: json
        })
      }
    })
  }
}
</script>
