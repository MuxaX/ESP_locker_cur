<template>
  <div class="key-management">
    <h2 class="page-title">Управление привязкой ключей</h2>

    <div class="controls">
      <div class="search-box">
        <input
          v-model="searchQuery"
          type="text"
          placeholder="Поиск по ФИО"
          class="search-input"
        />
      </div>
      <button
        @click="saveChanges"
        class="save-button"
        :disabled="!hasChanges || saving"
      >
        {{ saving ? "Сохранение..." : "Сохранить изменения" }}
      </button>
    </div>

    <div v-if="loading" class="loading-state">
      <div class="spinner"></div>
      <span>Загрузка данных...</span>
    </div>

    <div v-else-if="error" class="error-state">
      <span class="error-icon">!</span>
      <span>{{ error }}</span>
    </div>

    <div v-else>
      <div class="user-list">
        <div class="user-item" v-for="user in filteredUsers" :key="user.id">
          <div class="user-info">
            <span class="user-name">{{ user.full_name }}</span>
            <span class="current-key" v-if="user.key">
              Текущий ключ: <strong>{{ user.key.uid }}</strong>
            </span>
            <span class="no-key" v-else>Ключ не привязан</span>
          </div>

          <div class="key-selection">
            <select
              v-model="user.key_id"
              class="key-combobox"
              @change="markAsChanged(user.id)"
            >
              <option :value="null">-- Ключ не выбран --</option>
              <option
                v-for="key in availableKeys"
                :value="key.id"
                :key="key.id"
                :disabled="isKeyAssigned(key.id, user.id)"
              >
                {{ key.uid }}
              </option>
            </select>
          </div>
        </div>
      </div>

      <div v-if="filteredUsers.length === 0" class="empty-state">
        <span class="empty-icon">👤</span>
        <span>Пользователи не найдены</span>
      </div>
    </div>
  </div>
</template>

<script>
import EspData from "../services/data_import";

export default {
  name: "KeyManagement",
  data() {
    return {
      users: [],
      keys: [],
      loading: true,
      saving: false,
      error: null,
      searchQuery: "",
      changedUsers: new Set(),
    };
  },
  computed: {
    filteredUsers() {
      const query = this.searchQuery.toLowerCase();
      return this.users.filter((user) =>
        user.full_name.toLowerCase().includes(query)
      );
    },
    availableKeys() {
      return this.keys;
    },
    hasChanges() {
      return this.changedUsers.size > 0;
    },
  },
  methods: {
    async fetchData() {
      this.loading = true;
      this.error = null;
      try {
        const [keysResponse, peopleResponse] = await Promise.all([
          EspData.getKey(),
          EspData.getPeople(),
        ]);

        this.keys = keysResponse.data;
        this.users = peopleResponse.data.map((user) => ({
          ...user,
          // Обеспечиваем что key_id либо число, либо null
          key_id: user.key_id ? parseInt(user.key_id) : null,
        }));

        this.changedUsers = new Set();
      } catch (err) {
        this.error = "Не удалось загрузить данные";
        console.error("Ошибка:", err);
      } finally {
        this.loading = false;
      }
    },
    isKeyAssigned(keyId, currentUserId) {
      if (!keyId) return false;
      return this.users.some(
        (user) => user.key_id === keyId && user.id !== currentUserId
      );
    },
    markAsChanged(userId) {
      this.changedUsers.add(userId);
    },
    async saveChanges() {
      if (!this.hasChanges) return;

      this.saving = true;
      this.error = null;

      try {
        const updates = Array.from(this.changedUsers).map((userId) => {
          const user = this.users.find((u) => u.id === userId);
          return { userId, keyId: user.key_id };
        });

        for (const { userId, keyId } of updates) {
          try {
            await EspData.updateKey(userId, { key_id: keyId });
          } catch (err) {
            if (err.response?.status === 422) {
              alert(`Ошибка: ${err.response.data.message}`);
              return;
            }
            console.error("Ошибка при обновлении:", err);
            throw new Error(err.response?.data?.message || "Ошибка сервера");
          }
        }

        // Обновляем данные после успешного сохранения
        await this.fetchData();
        alert("Изменения успешно сохранены");
      } catch (err) {
        this.error = err.message;
        alert("Ошибка: " + this.error);
      } finally {
        this.saving = false;
      }
    },
  },
  mounted() {
    this.fetchData();
  },
};
</script>

<style scoped>
/* Ваши стили остаются без изменений */
.key-management {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
  font-family: "Arial", sans-serif;
}

.page-title {
  text-align: center;
  margin-bottom: 25px;
  color: #2c3e50;
  font-size: 24px;
}

.controls {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
  gap: 15px;
  flex-wrap: wrap;
}

.search-box {
  flex-grow: 1;
  min-width: 250px;
}

.search-input {
  width: 100%;
  padding: 10px 15px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 16px;
  transition: border-color 0.3s;
}

.search-input:focus {
  border-color: #3498db;
  outline: none;
}

.save-button {
  padding: 10px 20px;
  background-color: #2ecc71;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 16px;
  transition: background-color 0.3s;
  white-space: nowrap;
}

.save-button:hover:not(:disabled) {
  background-color: #27ae60;
}

.save-button:disabled {
  background-color: #95a5a6;
  cursor: not-allowed;
}

.user-list {
  display: flex;
  flex-direction: column;
  gap: 12px;
  margin-top: 20px;
}

.user-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 15px;
  background: white;
  border-radius: 6px;
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
  transition: box-shadow 0.3s;
}

.user-item:hover {
  box-shadow: 0 3px 6px rgba(0, 0, 0, 0.1);
}

.user-info {
  display: flex;
  flex-direction: column;
  gap: 5px;
  flex-grow: 1;
}

.user-name {
  font-weight: 600;
  font-size: 16px;
  color: #2c3e50;
}

.current-key,
.no-key {
  font-size: 14px;
}

.current-key {
  color: #3498db;
}

.no-key {
  color: #7f8c8d;
  font-style: italic;
}

.key-selection {
  min-width: 250px;
  margin-left: 20px;
}

.key-combobox {
  width: 100%;
  padding: 8px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 14px;
  background-color: white;
  transition: border-color 0.3s;
}

.key-combobox:focus {
  border-color: #3498db;
  outline: none;
}

.loading-state,
.error-state,
.empty-state {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 30px;
  margin-top: 20px;
  border-radius: 8px;
  text-align: center;
}

.loading-state {
  background-color: #f8f9fa;
  color: #3498db;
}

.error-state {
  background-color: #fdecea;
  color: #e74c3c;
}

.empty-state {
  background-color: #f8f9fa;
  color: #7f8c8d;
}

.spinner {
  border: 3px solid rgba(52, 152, 219, 0.2);
  border-top-color: #3498db;
  border-radius: 50%;
  width: 24px;
  height: 24px;
  animation: spin 1s linear infinite;
  margin-bottom: 10px;
}

.error-icon,
.empty-icon {
  font-size: 24px;
  margin-bottom: 10px;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

@media (max-width: 768px) {
  .user-item {
    flex-direction: column;
    align-items: flex-start;
    gap: 10px;
  }

  .key-selection {
    margin-left: 0;
    width: 100%;
  }

  .controls {
    flex-direction: column;
    align-items: stretch;
  }
}
</style>
