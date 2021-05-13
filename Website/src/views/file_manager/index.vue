<template>
  <div class="app-container">
    <el-form :inline="true" style="margin-bottom: -20px;">
      <el-form-item>
        <h2>Manage your Illumination Patterns</h2>
      </el-form-item>

      <el-form-item style="margin-top: 12px;">
        <el-upload
          ref="upload"
          action="/upload"
          accept=".csv"
          show-file-list="false"
          :before-upload="submitUpload"
        >
          <el-button size="medium" type="primary">Click to upload</el-button>
        </el-upload>
      </el-form-item>
    </el-form>

    <el-table :data=" $store.state.esp_data.file_list" style="width: 100%">
      <el-table-column label="Pattern file" prop="filename" />
      <el-table-column label="Operations">
        <template slot-scope="scope">

          <el-button icon="el-icon-download" @click="download_file(scope.row.file, scope.row.filename)">Download</el-button>

          <el-button icon="el-icon-delte" type="danger" @click="handleDelete(scope.row.file)">Delete</el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script>
import Socket from '@/socket'
import axios from 'axios'
export default {
  data() {
    return {}
  },
  mounted() {},
  methods: {
    download_file(file, filename) {
      axios({
        url: file,
        method: 'GET',
        responseType: 'blob'
      }).then((response) => {
        var fileURL = window.URL.createObjectURL(new Blob([response.data]))
        var fileLink = document.createElement('a')
        fileLink.href = fileURL
        fileLink.setAttribute('download', filename + '.csv')
        document.body.appendChild(fileLink)
        fileLink.click()
      })
    },
    submitUpload(file_uploaded) {
      var file_path
      var file_name
      var shorted
      if (file_uploaded.name.length > 22) {
        file_path = '/conf/' + file_uploaded.name.slice(0, 20)
        file_name = file_uploaded.name.slice(0, 20)
        shorted = true
      } else {
        file_path = '/conf/' + file_uploaded.name
        file_name = file_uploaded.name.slice(0, -4)
      }
      if (shorted) {
        this.$message({
          message:
            'The file name of the patternis too long (max 20 characters), it will be shortened',
          type: 'warning'
        })
        this.$store.commit('add_file', {
          file: file_path,
          filename: file_name
        })
      } else if (
        this.$store.state.esp_data.file_list.some(
          (file_list) => file_list.file === file_path
        )
      ) {
        this.$message({
          message:
            'A file with the same name have already been uploaded in the past, the pattern will be overwritten',
          type: 'warning'
        })
      } else {
        this.$message({
          message: 'Patten has been uploaded, you can load it now',
          type: 'success'
        })
        this.$store.commit('add_file', {
          file: file_path,
          filename: file_name
        })
      }
    },
    handleDownload(file) {
      var element = document.createElement('el-link')
      element.setAttribute('href', file)
      element.setAttribute('download')
      element.style.display = 'none'
      document.body.appendChild(element)
      element.click()
      document.body.removeChild(element)
    },
    handleDelete(file_to_delete) {
      Socket.send(
        JSON.stringify({ command: 'delete_file', file: file_to_delete })
      )
      this.$store.commit('delete_one_file', {
        file_to_be_deleted: file_to_delete
      })
    }
  }
}
</script>
