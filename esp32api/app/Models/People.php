<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Model;

class People extends Model
{

    protected $table = 'peoples';
    protected $fillable = [
        'last_name',
        'first_name',
        'middle_name',
        'key_id'
    ];

    public function getFullNameAttribute()
    {
        return trim("{$this->last_name} {$this->first_name} " . ($this->middle_name ?? ''));
    }

    public function key()
    {
        return $this->belongsTo(Key::class);
    }
}
