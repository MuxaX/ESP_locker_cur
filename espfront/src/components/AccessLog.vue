<template>
  <div class="access-logs">
    <div class="header-section">
      <h2 class="page-title">Журнал доступа к локерам</h2>
      <button class="nav-button" @click="goToKeyManagement">
        Управление ключами
      </button>
    </div>

    <div class="table-wrapper">
      <table class="modern-table">
        <thead>
          <tr class="table-header">
            <th class="header-cell">№ Локера</th>
            <th class="header-cell">Ключ доступа</th>
            <th class="header-cell">Закреплён за</th>
            <th class="header-cell">Статус</th>
            <th class="header-cell">Дата и время</th>
          </tr>
        </thead>
        <tbody>
          <tr
            v-for="(log, index) in sortedLogs"
            :key="index"
            class="table-row"
            :class="{ 'last-row': index === sortedLogs.length - 1 }"
          >
            <td class="data-cell reader-cell">
              <span
                class="reader-badge"
                :class="{
                  'reader-open': log.door_status === 'Open',
                  'reader-closed': log.door_status === 'Closed',
                }"
              >
                {{ log.reader || "—" }}
              </span>
            </td>
            <td class="data-cell uid-cell">
              {{ log.uid?.toUpperCase() || "—" }}
            </td>
            <td class="data-cell person-cell">
              {{ log.person || "Не закреплён" }}
            </td>
            <td class="data-cell">
              <span :class="statusClass(log.status)">
                {{ getStatusText(log.status) }}
              </span>
            </td>
            <td class="data-cell date-cell">
              {{ formatDateTimeUTC(log.timestamp) }}
            </td>
          </tr>
        </tbody>
      </table>
    </div>

    <div v-if="loading" class="loading-state">
      <div class="spinner"></div>
      <span>Загрузка данных...</span>
    </div>

    <div v-if="error" class="error-state">
      <span class="error-icon">!</span>
      <span>{{ error }}</span>
    </div>

    <div v-if="!loading && logs.length === 0" class="empty-state">
      <span class="empty-icon">📭</span>
      <span>Нет данных для отображения</span>
    </div>
  </div>
</template>

<script>
import EspData from "../services/data_import";

export default {
  name: "AccessLog",
  data() {
    return {
      logs: [],
      loading: false,
      error: null,
    };
  },
  computed: {
    sortedLogs() {
      return [...this.logs].sort(
        (a, b) => new Date(b.timestamp) - new Date(a.timestamp)
      );
    },
  },
  methods: {
    formatDateTimeUTC(dateString) {
      if (!dateString) return "—";
      const date = new Date(dateString);
      return date.toLocaleString("ru-RU", {
        day: "2-digit",
        month: "2-digit",
        year: "numeric",
        hour: "2-digit",
        minute: "2-digit",
        second: "2-digit",
      });
    },
    getStatusText(status) {
      return status === "Access Complete"
        ? "Доступ получен"
        : status === "Access Denied"
        ? "Отказано в доступе"
        : "Неизвестный статус";
    },
    statusClass(status) {
      return {
        "status-success": status === "Access Complete",
        "status-error": status === "Access Denied",
        "status-unknown": !["Access Complete", "Access Denied"].includes(
          status
        ),
      };
    },
    async fetchLogs() {
      this.loading = true;
      this.error = null;
      try {
        const response = await EspData.getLog();
        this.logs = response.data || [];
      } catch (error) {
        this.error =
          error.response?.data?.message ||
          "Ошибка при загрузке журнала доступа";
        console.error("Ошибка:", error);
      } finally {
        this.loading = false;
      }
    },
    goToKeyManagement() {
      this.$router.push("/KeyManagement");
    },
  },
  mounted() {
    this.fetchLogs();
  },
};
</script>

<style scoped>
.header-section {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 2rem;
  flex-wrap: wrap;
  gap: 1rem;
}

.nav-button {
  background: linear-gradient(90deg, #4361ee 0%, #3a0ca3 100%);
  color: white;
  border: none;
  padding: 0.75rem 1.5rem;
  border-radius: 8px;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.3s ease;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  font-family: inherit;
}

.nav-button:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15);
}

.nav-button:active {
  transform: translateY(0);
}

.access-logs {
  max-width: 1200px;
  margin: 0 auto;
  padding: 2rem;
  font-family: "Inter", -apple-system, BlinkMacSystemFont, sans-serif;
}

.page-title {
  color: #1a1a1a;
  font-size: 2rem;
  font-weight: 700;
  letter-spacing: -0.5px;
  margin: 0;
}

.table-wrapper {
  background: white;
  border-radius: 16px;
  box-shadow: 0 10px 30px rgba(0, 0, 0, 0.05);
  overflow: hidden;
  border: 1px solid #f0f0f0;
}

.modern-table {
  width: 100%;
  border-collapse: separate;
  border-spacing: 0;
}

.table-header {
  background: linear-gradient(90deg, #4361ee 0%, #3a0ca3 100%);
  color: white;
}

.header-cell {
  padding: 1.25rem 1.5rem;
  text-align: left;
  font-weight: 600;
  font-size: 0.9rem;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.header-cell:first-child {
  border-top-left-radius: 16px;
}

.header-cell:last-child {
  border-top-right-radius: 16px;
}

.table-row {
  transition: all 0.25s ease;
  border-bottom: 1px solid #f5f5f5;
}

.table-row:hover {
  background-color: #f8faff;
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
}

.data-cell {
  padding: 1.25rem 1.5rem;
  color: #333;
  font-size: 0.95rem;
}

.reader-cell {
  font-weight: 600;
}

.reader-open {
  background: #e6f7ee !important;
  color: #00a854 !important;
  border: 1px solid #00a854;
}

.reader-closed {
  background: #ffebee !important;
  color: #f44336 !important;
  border: 1px solid #f44336;
}

.reader-badge {
  display: inline-block;
  padding: 0.4rem 0.8rem;
  background: #f0f4ff;
  color: #4361ee;
  border-radius: 12px;
  font-size: 0.85rem;
  font-weight: 600;
  border: 1px solid #4361ee;
  transition: all 0.3s ease;
}

.uid-cell {
  font-family: "Fira Code", monospace;
  color: #555;
  letter-spacing: 0.5px;
}

.status-success {
  background: #e6f7ee;
  color: #00a854;
}

.status-error {
  background: #ffebee;
  color: #f44336;
}

.status-unknown {
  background: #f5f5f5;
  color: #666;
}

.date-cell {
  color: #666;
  font-size: 0.9rem;
}

.last-row {
  border-bottom: none;
}

.loading-state,
.error-state,
.empty-state {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 0.8rem;
  padding: 1.5rem;
  margin-top: 2rem;
  border-radius: 12px;
  font-size: 1rem;
  font-weight: 500;
}

.loading-state {
  background: #f5f9ff;
  color: #4361ee;
}

.error-state {
  background: #fff5f5;
  color: #f44336;
}

.empty-state {
  background: #f9f9f9;
  color: #666;
}

.spinner {
  width: 1.25rem;
  height: 1.25rem;
  border: 3px solid rgba(67, 97, 238, 0.2);
  border-top-color: #4361ee;
  border-radius: 50%;
  animation: spin 1s linear infinite;
}

.error-icon,
.empty-icon {
  font-size: 1.2rem;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}

.table-row {
  animation: fadeIn 0.4s ease forwards;
  opacity: 0;
}

@keyframes fadeIn {
  to {
    opacity: 1;
  }
}

@media (max-width: 768px) {
  .header-section {
    flex-direction: column;
    align-items: flex-start;
  }

  .access-logs {
    padding: 1rem;
  }

  .header-cell,
  .data-cell {
    padding: 1rem;
    font-size: 0.85rem;
  }

  .reader-badge {
    padding: 0.3rem 0.6rem;
    font-size: 0.8rem;
  }

  .page-title {
    font-size: 1.5rem;
  }
}
</style>
