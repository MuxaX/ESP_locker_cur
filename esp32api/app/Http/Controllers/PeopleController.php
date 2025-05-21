<?php

namespace App\Http\Controllers;

use App\Models\People;
use App\Models\Key;
use Illuminate\Http\Request;
use Illuminate\Validation\Rule;
use Illuminate\Support\Facades\DB;

class PeopleController extends Controller
{
 public function updateKey(Request $request, $id)
{
    $validated = $request->validate([
        'key_id' => 'nullable|integer|exists:keys,id'
    ]);

    // Проверка на уже привязанный ключ
    if ($validated['key_id'] && People::where('key_id', $validated['key_id'])
                                   ->where('id', '!=', $id)
                                   ->exists()) {
        return response()->json([
            'success' => false,
            'message' => 'Этот ключ уже используется другим человеком'
        ], 422);
    }

    // Используем query builder вместо модели
    DB::table('peoples')
        ->where('id', $id)
        ->update(['key_id' => $validated['key_id']]);

    // Возвращаем обновленные данные
    $person = People::with('key')->find($id);
    
    return response()->json([
        'success' => true,
        'message' => 'Ключ успешно обновлен',
        'person' => $person
    ]);
}


    public function KeyList()
    {
        return Key::select('id', 'uid')->get();
    }

    public function PeopleList()
    {
    return People::with('key')->get()->map(function($person) {
        return [
            'id' => $person->id,
            'full_name' => $person->last_name . ' ' . $person->first_name . ' ' . ($person->middle_name ?? ''),
            'key_id' => $person->key_id,
            'key' => $person->key
        ];
    });
    }
}
