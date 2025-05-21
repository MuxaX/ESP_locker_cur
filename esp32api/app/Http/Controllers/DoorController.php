<?php

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\Models\DoorEvent;
use Illuminate\Support\Facades\Log;

class DoorController extends Controller
{
    public function update(Request $request)
    {
        try {
            $validated = $request->validate([
                'status' => 'required|string|in:Open,Closed',
                'reader' => 'required|integer|min:0|max:2',
            ]);

            // Находим или создаем запись
            $doorEvent = DoorEvent::updateOrCreate(
                ['reader' => $validated['reader']],
                ['status' => $validated['status']]
            );

            Log::info('Door status updated', [
                'status' => $doorEvent->status,
                'reader' => $doorEvent->reader
            ]);

            return response()->json([
                'success' => true,
                'message' => 'Door status successfully updated',
                'data' => $doorEvent
            ], 200);

        } catch (\Exception $e) {
            Log::error('Door controller error: ' . $e->getMessage());
            return response()->json([
                'success' => false,
                'message' => 'Server error',
                'error' => $e->getMessage()
            ], 500);
        }
    }
}