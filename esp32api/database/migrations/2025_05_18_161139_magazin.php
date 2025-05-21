<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

return new class extends Migration
{
    /**
     * Run the migrations.
     */
    public function up(): void
    {
        Schema::create('magazin', function (Blueprint $table) {
            $table->id();
    

            $table->foreignId('people_id')
            ->nullable()
            ->constrained('peoples')
            ->nullOnDelete();


            $table->foreignId('log_id')
            ->nullable()
            ->constrained('access_logs')
            ->nullOnDelete();
        });
    }

    /**
     * Reverse the migrations.
     */
    public function down(): void
    {
        //
    }
};
