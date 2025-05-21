<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Log;
use App\Models\AccessLog;
use Carbon\Carbon;
use App\Models\DoorEvent;

class AccessLogController extends Controller
{
    public function createLog(Request $request)
    {
        // Проверяем обязательные поля
        $requiredFields = ['status', 'uid'];
        foreach ($requiredFields as $field) {
            if (!$request->has($field)) {
                return response()->json(['error' => "Отсутствует обязательное поле: $field"], 400);
            }
        }

        // Логируем данные
        Log::info('Данные с ESP32:', $request->all());

        // Сохраняем в базу
        $log = AccessLog::create([
            'status' => $request->input('status'),
            'uid' => $request->input('uid'),
            'reader' => $request->input('reader'), // Может быть null
            'timestamp' => now()->addHours(5), // автоматически сохраняется в UTC
        ]);

        return response()->json([
            'message' => 'Данные сохранены',
            'data' => $log
        ]);
    }

    public function getLog()
    {
        $logs = AccessLog::with(['magazin.people'])
            ->orderBy('timestamp', 'desc')
            ->get()
            ->map(function ($log) {
                $personName = null;
                
                if ($log->magazin && $log->magazin->people) {
                    $p = $log->magazin->people;
                    $personName = trim("{$p->last_name} {$p->first_name} " . ($p->middle_name ?? ''));
                }

                // Получаем текущий статус двери для этого ридера
                $doorStatus = DoorEvent::where('reader', $log->reader)
                    ->value('status'); // Получаем только значение статуса
                
                return [
                    'id' => $log->id,
                    'status' => $log->status,
                    'uid' => $log->uid,
                    'reader' => $log->reader,
                    'timestamp' => $log->timestamp,
                    'person' => $personName,
                    'door_status' => $doorStatus ?? 'Unknown', // Статус двери или 'Unknown' если нет записи
                ];
            });

        return response()->json($logs);
    }
    
    public function getLogByReader($reader)
    {
        $logs = AccessLog::where('reader', $reader)
            ->orderBy('timestamp', 'desc')
            ->get();
            
        return response()->json($logs);
    }
}