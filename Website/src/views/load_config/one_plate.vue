<template>
  <div class="app-container">
    <el-form
      ref="plate_A"
      label-width="180px"
      label-position="left"
      :model="send_to_esp.plate_A"
      :rules="rules"
    >
      <h3>Center / Corner plate</h3>
      <el-form-item label="Pattern:" prop="file">
        <el-select
          v-model="send_to_esp.plate_A.file"
          clearable
          size="large"
          style="width: 100%;"
          placeholder="Please select an illumination pattern"
        >
          <el-option
            v-for="item in $store.state.esp_data.file_list"
            :key="item.file"
            :label="item.filename"
            :value="item.file"
          />
        </el-select>
      </el-form-item>

      <el-form-item label="Format:" prop="wellplate">
        <el-select
          v-model="send_to_esp.plate_A.wellplate"
          style="width: 100%;"
          clearable
          placeholder="Please select the microplate that you are using"
        >
          <el-option
            v-for="item in wellplate_options_a"
            :key="item.value"
            :label="item.label"
            :value="item.value"
          />
        </el-select>
      </el-form-item>
      <el-form-item label="Position:" prop="position">
        <el-select
          v-model="wellplate_position"
          style="width: 100%;"
          clearable
          placeholder="Please select the position of the microplate"
        >
          <el-option
            v-for="item in wellplate_pos"
            :key="item.value"
            :label="item.label"
            :value="item.value"
          />
        </el-select>
      </el-form-item>
      <el-form-item label="Illumination range:">
        <el-switch v-model="send_to_esp.plate_A.custom_offset" />
      </el-form-item>
      <div v-if="send_to_esp.plate_A.custom_offset" style="padding-left: 180px">
        <el-form
          :inline="true"
          label-width="130px"
          label-position="left"
          :model="plate_A"
          :rules="rules"
        >
          <el-form-item label="Start column:">
            <el-input-number v-model="send_to_esp.plate_A.min_col" :min="1" :max="12" />
          </el-form-item>
          <el-form-item label="Start row:">
            <el-input v-model="send_to_esp.plate_A.min_row" required pattern="[A-Ha-h]" />
          </el-form-item>
        </el-form>
        <el-form :inline="true" label-width="130px" label-position="left">
          <el-form-item label="End column:">
            <el-input-number v-model="send_to_esp.plate_A.max_col" :min="1" :max="12" />
          </el-form-item>

          <el-form-item label="End row:">
            <el-input v-model="send_to_esp.plate_A.max_row" required pattern="[A-Ha-h]" />
          </el-form-item>
        </el-form>
      </div>

      <el-form-item>
        <el-button type="primary" @click="submitForm('plate_A')">Load Pattern</el-button>
        <el-button @click="resetForm('plate_A')">Cancel</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
import Socket from '@/socket'
export default {
  data() {
    var file_a_validator = (rule, value, callback) => {
      if (this.send_to_esp.plate_A.file === '') {
        callback(new Error('Please select an illumination pattern'))
      } else {
        callback()
      }
    }
    var wellplate_a_validator = (rule, value, callback) => {
      if (this.send_to_esp.plate_A.wellplate === '') {
        callback(new Error('Please choose a microplate format'))
      } else {
        callback()
      }
    }
    var position_a_validator = (rule, value, callback) => {
      if (this.wellplate_position === null) {
        callback(new Error('Please choose a position'))
      } else {
        callback()
      }
    }

    return {
      wellplate_options_a: [
        {
          value: 1,
          label: '96 wells'
        },
        {
          value: 2,
          label: '48 wells'
        },
        {
          value: 3,
          label: '24 wells'
        },
        {
          value: 4,
          label: '12 wells'
        },
        {
          value: 5,
          label: '6 wells'
        }
      ],
      wellplate_position: null,
      wellplate_pos:
      [
        {
          value: 1,
          label: 'Center'
        },
        {
          value: 2,
          label: 'Corner'
        }
      ],
      send_to_esp: {
        command: 'load_config',
        two_wellplates: false,
        plate_A: {
          file: '',
          wellplate: '',
          custom_offset: false,
          min_col: 1,
          max_col: 12,
          min_row: 'A',
          max_row: 'H'
        }
      },

      rules: {
        file: [
          {
            validator: file_a_validator,
            trigger: 'blur'
          }
        ],
        wellplate: [
          {
            validator: wellplate_a_validator,
            trigger: 'blur'
          }
        ],
        position: [
          {
            validator: position_a_validator,
            trigger: 'blur'
          }
        ]
      }
    }
  },
  computed: {},
  mounted() {
    this.calc_wellplate_options_b()
  },
  methods: {
    submitForm(formName) {
      this.$refs[formName].validate(valid => {
        if (valid) {
          if (this.wellplate_position === 2) {
            this.send_to_esp.plate_A.wellplate += 50
          }
          Socket.send(JSON.stringify(this.send_to_esp))
          this.$message({
            message:
              'Pattern has been loaded, you can now use the device',
            type: 'success'
          })
        } else {
          console.log('error submit!!')
          return false
        }
      })
    },

    resetForm(formName) {
      this.$message({
        message: 'Form has been cleared',
        type: 'message'
      })
      this.$refs[formName].resetFields()
      if (formName === 'plate_A') {
        this.wellplate_position = null
        this.send_to_esp.plate_A.custom_offset = false
        this.send_to_esp.plate_A.min_col = 1
        this.send_to_esp.plate_A.min_row = 'A'
        this.send_to_esp.plate_A.max_col = 12
        this.send_to_esp.plate_A.max_row = 'H'
      }
    }
  }
}
</script>

